#ifndef H_LINKLIST
#define H_LINKLIST

/**
 * @brief struct sLinkListNode represents a node in the link list
 */
typedef struct sLinkListNode
{
    void *data;                 /**< @brief The data */
    struct sLinkListNode *next; /**< @brief The next element */
}SLinkListNode;

/**
 * @brief struct sLinkList represents the link list
 */
typedef struct sLinkList
{
    SLinkListNode *start;        /**< @brief Start node of list */

    //Function pointers
    void (*destroy) (void *);    /**< @brief Delete the nodes */
    void (*print) (void*);       /**< @brief Print out */
}SLinkList;

/**
 * @brief Create a link list
 * @param print print a node in the link list
 * @param destroy a node in the link list
 * @return The created link list
 */
SLinkList *linkList_create (void (*print) (void*),
                            void (*destroy) (void*));

/**
 * @brief Destroy a link list
 * @param list the link list we wish to destroy
 */
void linkList_destroy (SLinkList *list);

/**
 * @brief Add to the start
 * @param list the link list
 * @param elem the element we wish to add
 */
void linkList_addHead (SLinkList *list, void *elem);

/**
 * @brief Remove the head
 * @param list the link list
 */
void linkList_removeHead (SLinkList *list);

/**
 * @brief Retrieve the head
 * @param list the link list
 * @return the head of the link list, can be NULL
 */
void* linkList_getHead (SLinkList *list);

/**
 * @brief Is link list empty?
 * @param list the link list
 * @return whether or not linklist is empty
 */
int linkList_isEmpty (SLinkList *list);
#endif
