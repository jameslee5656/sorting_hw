#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define gnBARBER_NUM    3
#define gnTOTAL_SEATS   2
#define gsEVENT_QUEUE_NAME  "./sleeping_barber_event_queue"
#define gnEVENT_MTYPE   1

#define ERROR_PTHREAD_CREATION_FAILURE      "[Error] pthread creation failure"
#define ERROR_SOFA_IS_FULL                  "[Error] 沙發滿了! 客人請離開\n"
#define ERROR_EVENT_QUEUE_CREATION_FAILURE  "[Error] Event Queue 建立失敗\n"
#define ERROR_EVENT_QUEUE_SEND_FAILURE      "[Error] Event Queue 傳送訊息失敗\n"
#define ERROR_EVENT_QUEUE_RECV_FAILURE      "[Error] Event Queue 收到訊息失敗\n"

enum eventType_t
{
    customer,
    ending,
};

struct eventMessage_t
{
    long int mtype;
    enum eventType_t eventType;
    int nNeedSeconds;
};

#endif // DEFINITIONS_H