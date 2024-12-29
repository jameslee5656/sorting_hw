#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bibleSearch.hpp"

int gnPatternCount = 0;

typedef struct _node_t
{
    int isLeaf;
    int nPattern; // which pattern index it is
    char sContent[gPATTERN_MAX_LEN];
    struct _node_t *pNextArr[gASCII_NUM];
    struct _node_t *pFailureLink;
} node;

int buildTree(node* pRoot, char *psPattern)
{
    int i = 0, nPatternLen = 0;
    unsigned int uKey = 0;
    node* pNode;

    // 1. check pattern is not empty
    if(psPattern == NULL)
        return gERROR_GENERAL;

    nPatternLen = strnlen(psPattern, gPATTERN_MAX_LEN);

    // 2. check pRoot exist or not
    if (pRoot == NULL)
        return gERROR_ARGUMENT_INCORRECT;

    pNode = pRoot;
    // 3. 找尋已存在的節點
    for (i = 0; i < nPatternLen; ++i)
    {
        uKey = (unsigned char) psPattern[i];
        // 3.1 該字節點存在
        if(pNode->pNextArr[uKey])
            pNode = pNode->pNextArr[uKey];
        // 3.2 該字節點不存在
        else
        {
            // 建立節點並初始化
            pNode->pNextArr[uKey] = (node *) malloc(sizeof(node));
            pNode = pNode->pNextArr[uKey];
            memset(pNode, 0, sizeof(node));
            strncpy(pNode->sContent, psPattern, nPatternLen - 1);
            pNode->sContent[i + 1] = '\0';
        }
    }

    // 4. 處理結尾node
    pNode->isLeaf = 1;
    pNode->nPattern = ++gnPatternCount;

    return gSUCCESS;
}


// 最長尾巴等於別人的開頭
int buildFailureLink(node* pCurNode, node *pRoot)
{
    char sPatternTmp[gPATTERN_ARR_MAX_SIZE];
    unsigned char uc;
    node *pNode = NULL;
    int i;

    if(pCurNode == NULL)
        return gERROR_ARGUMENT_INCORRECT;

    // 使用 DFS(深度優先尋找 )，將樹的每一條路都走到底
    for(i = 0; i < gASCII_NUM; ++i)
    {
        if(pCurNode->pNextArr[i] != NULL)
            buildFailureLink(pCurNode->pNextArr[i], pRoot);
    }

    // 1. 確認該節點的字元不得為結尾，以此確認這條路是通的
    if(pCurNode->sContent[0] != '\0')
    {
        // 2. 取出當前節點的 Pattern(將當前該節點的字串內容複製到 szTmp)
        strncpy(sPatternTmp, pCurNode->sContent, gPATTERN_MAX_LEN - 1);
        sPatternTmp[gPATTERN_MAX_LEN - 1] = '\0';

        // 3. while 要比對的是尾巴，取得字串後暫存於sPatternTmp
        while(strlen(sPatternTmp) > 0)
        {
            strncpy(sPatternTmp, &sPatternTmp[1], gPATTERN_MAX_LEN - 1);
            sPatternTmp[gPATTERN_MAX_LEN - 1] = '\0';
            if (sPatternTmp[0] == '\0')
                break;

            // 3.2 從 root 開始尋找是否有相同於 szTmp的字串
            pNode = pRoot;
            for(i = 0; i < gPATTERN_MAX_LEN; ++i)
            {
                if(sPatternTmp[i] == '\0')
                    break;
                uc = (unsigned char) sPatternTmp[i];
                if(pNode == NULL)
                    break;
                if(pNode->pNextArr[uc] == NULL)
                    break;
                pNode = pNode->pNextArr[uc];
            }

            // 4. 找到了 pFailureLink 指向 找到的節點
            // pattern比對 到結尾字串，代表找到了
            if(sPatternTmp[i] == '\0')
            {
                pCurNode->pFailureLink = pNode;
                break;
            }
        }

        // 5. 沒找到， pFailureLink 指向 root
        // pattern(szTmp)被 移除 只剩結尾字元
        if (sPatternTmp[0] == '\0')
            pCurNode->pFailureLink = pRoot;
    }

    return gSUCCESS;
}

void checkFailureLinkEnd(node* pCurNode, node *pRoot, int *acResult)
{
    int nResult = 0;

    if (pCurNode->pFailureLink && pCurNode->pFailureLink != pRoot)
        checkFailureLinkEnd(pCurNode->pFailureLink, pRoot, acResult);

    if (pCurNode->isLeaf)
        ++acResult[pCurNode->nPattern];

    return;
}

int traversal(node* pRoot, char *psBible, int *acResult)
{
    int nBibleIdx = 0, nBibleLen = 0;
    int nResult = 0;
    unsigned int uKey = 0;
    node *pNode = NULL;

    if (pRoot == NULL || psBible == NULL || acResult == NULL)
        return gERROR_GENERAL;

    nBibleLen = strnlen(psBible, gBIBLE_MAX_LEN);
    pNode = pRoot;

    // 1. 搜尋整份文件
    uKey = (unsigned char) psBible[nBibleIdx++];
    while (nBibleIdx < nBibleLen)
    {
        // 2. if (ptr->pNextArr[c] == NULL)
        if (pNode->pNextArr[uKey] == NULL)
        {
            // 2.1 if ptr == pRoot, move on to pNextArr char
            if (pNode == pRoot)
                uKey = (unsigned char) psBible[nBibleIdx++];
            // 2.2 else ptr = ptr->failure
            else
                pNode = pNode->pFailureLink;
        }
        // 3. else have the pNextArr point
        else
        {
            pNode = pNode->pNextArr[uKey];

            // 3.1 如果是最後一個點，pattern 的結果 +1
            if (pNode->isLeaf)
            {
                // printf("found leaf! %d\n", pNode->nPattern);
                ++acResult[pNode->nPattern];
            }

            // 3.2 check failure link has an end
            checkFailureLinkEnd(pNode, pRoot, acResult);

            // 3.3 移動到下一個點
            uKey = (unsigned char) psBible[nBibleIdx++];
        }

        // if (nBibleIdx % 10000 == 0)
            // printf("read bible %d %c\n", nBibleIdx, psBible[nBibleIdx]);
    }

    return gSUCCESS;
}

int acSearch(char **psPatternArr, int nPatternArrSz, char *psBibleContent, int *acResult)
{
    int i = 0, nBibleLen = 0, ret = 0;
    node *treeRoot = NULL;

    if (psBibleContent == NULL || nPatternArrSz == 0 || psBibleContent == NULL || acResult == NULL)
        return gERROR_GENERAL;

    // Initialize variable
    gnPatternCount = -1;
    nBibleLen = strnlen(psBibleContent, gBIBLE_MAX_LEN);
    treeRoot = (node *) malloc(sizeof(node));
    memset(treeRoot, 0, sizeof(node));

    // 1. build tree with every pattern in patternArray
    for (i = 0; i < nPatternArrSz; ++i)
    {
        ret = buildTree(treeRoot, psPatternArr[i]);
        if (ret < 0)
        {
            printf(gERROR_MSG_AC_SEARCH_BUILD_FAILED, i);
            return ret;
        }
    }

    // 2. build failure link
    ret = buildFailureLink(treeRoot, treeRoot);
    if (ret < 0)
    {
        printf(gERROR_MSG_PREPROCESSING_FAILED, gAC_SEARCH_NAME, ret);
        return ret;
    }

    // 3. traverse the tree with bible content
    traversal(treeRoot, psBibleContent, acResult);

    if (treeRoot != NULL)
    {
        free(treeRoot);
        treeRoot = NULL;
    }

    return gSUCCESS;
}