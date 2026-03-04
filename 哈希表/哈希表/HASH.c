#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// 定义数据类型
typedef char KeyType;
typedef int ValueType;

typedef struct {
    KeyType key;
    ValueType value;
} DataType;

// 定义结点
typedef struct Node {
    DataType data;
    struct Node* next;
    struct Node* prior;
} Node;

// 创建结点
Node* createNode(DataType data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    newNode->data = data;
    newNode->next = NULL;
    newNode->prior = NULL;
    return newNode;
}

// 销毁结点
void destroyNode(Node* node) {
    free(node);
}

// 获得结点键
KeyType getNodeKey(Node* node) {
    return node->data.key;
}

// 获得结点值
ValueType getNodeValue(Node* node) {
    return node->data.value;
}

// 定义链表
typedef struct {
    Node* head;
    int size;
} LinkedList;

// 创建链表
LinkedList* createLinkedList() {
    LinkedList* newList = (LinkedList*)malloc(sizeof(LinkedList));
    if (newList == NULL) {
        printf("Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    newList->head = NULL;
    newList->size = 0;
    return newList;
}

// 销毁链表
void destroyLinkedList(LinkedList* list) {
    Node* current = list->head;
    while (current != NULL) {
        Node* temp = current;
        current = current->next;
        destroyNode(temp);
    }
    free(list);
}

// 向链表中添加结点
bool add(LinkedList* list, DataType data) {
    Node* newNode = createNode(data);
    if (list->head == NULL) {
        list->head = newNode;
    }
    else {
        Node* current = list->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
        newNode->prior = current;
    }
    list->size++;
    return true;
}

// 移除链表中的值为key的结点
bool removeByKey(LinkedList* list, KeyType key) {
    Node* current = list->head;
    while (current != NULL) {
        if (current->data.key == key) {
            if (current->prior != NULL) {
                current->prior->next = current->next;
            }
            else {
                list->head = current->next;
            }
            if (current->next != NULL) {
                current->next->prior = current->prior;
            }
            destroyNode(current);
            list->size--;
            return true;
        }
        current = current->next;
    }
    return false;
}

// 判断链表是否为空
bool isEmpty(LinkedList* list) {
    return list->size == 0;
}

// 统计此链表的元素数目
int size(LinkedList* list) {
    return list->size;
}

// 获取链表头节点的下一个节点（第一个元素）
Node* getFirst(LinkedList* list) {
    return list->head;
}

// 定义哈希表
typedef struct {
    int size;
    int capacity;
    float load_factor;
    LinkedList** array;
} HashTable;

// 哈希函数
int hashFunction(KeyType key, int capacity) {
    // 这里可以实现你的哈希函数，例如简单地取模
    return key % capacity;
}

// 初始化哈希表
int HashInitiate(HashTable* table, int capacity, float load_factor) {
    table->size = 0;
    table->capacity = capacity;
    table->load_factor = load_factor;
    table->array = (LinkedList**)malloc(capacity * sizeof(LinkedList*));
    if (table->array == NULL) {
        printf("Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < capacity; i++) {
        table->array[i] = createLinkedList();
    }
    return 1;
}

// 销毁哈希表
void destroyHashTable(HashTable* table) {
    for (int i = 0; i < table->capacity; i++) {
        destroyLinkedList(table->array[i]);
    }
    free(table->array);
}

// 判断哈希表是否为空
int isHashTableEmpty(HashTable* table) {
    return table->size == 0;
}

// 判断链表是否为空
bool isLinkedListEmpty(LinkedList* list) {
    return list->size == 0;
}


// 查找哈希表中key对应的value的地址
ValueType* HashFind(HashTable* table, KeyType key) {
    int index = hashFunction(key, table->capacity);
    Node* current = getFirst(table->array[index]);
    while (current != NULL) {
        if (getNodeKey(current) == key) {
            return &(current->data.value);
        }
        current = current->next;
    }
    return NULL;
}

// 在哈希表中查找桶号
int findBucket(HashTable* table, KeyType key) {
    return hashFunction(key, table->capacity);
}

// 删除哈希表中的记录
int HashDelete(HashTable* table, KeyType key) {
    int bucket = findBucket(table, key);
    if (bucket >= table->capacity) {
        return 0;
    }
    if (removeByKey(table->array[bucket], key)) {
        table->size--;
        return 1;
    }
    return 0;
}

// 插入记录到哈希表
int HashInsert(HashTable* table, DataType x) {
    int bucket = findBucket(table, x.key);
    if (bucket >= table->capacity) {
        return 0;
    }
    if (HashFind(table, x.key) != NULL) {
        // 如果已经存在相同的key，则更新value
        *HashFind(table, x.key) = x.value;
        return 1;
    }
    else {
        // 否则插入新的结点
        add(table->array[bucket], x);
        table->size++;
        // 检查是否需要重建哈希表
        if ((float)table->size / table->capacity > table->load_factor) {
            int new_capacity = 2 * table->capacity;
            LinkedList** new_array = (LinkedList**)malloc(new_capacity * sizeof(LinkedList*));
            if (new_array == NULL) {
                printf("Memory allocation failed\n");
                exit(EXIT_FAILURE);
            }
            for (int i = 0; i < new_capacity; i++) {
                new_array[i] = createLinkedList();
            }
            for (int i = 0; i < table->capacity; i++) {
                Node* current = getFirst(table->array[i]);
                while (current != NULL) {
                    DataType data = current->data;
                    int new_bucket = hashFunction(data.key, new_capacity);
                    add(new_array[new_bucket], data);
                    current = current->next;
                }
                destroyLinkedList(table->array[i]);
            }
            free(table->array);
            table->array = new_array;
            table->capacity = new_capacity;
        }
        return 1;
    }
}

int main() {
    // 示例用法
    HashTable table;
    HashInitiate(&table, 10, 1.5); // 初始化哈希表，容量为10，填充因子为1.5
    DataType data1 = { 'a', 1 };
    HashInsert(&table, data1); // 插入记录
    DataType data2 = { 'b', 2 };
    HashInsert(&table, data2);
    DataType data3 = { 'c', 3 };
    HashInsert(&table, data3);

    // 查找记录
    KeyType key_to_find = 'b';
    ValueType* value_found = HashFind(&table, key_to_find);
    if (value_found != NULL) {
        printf("Value found: %d\n", *value_found);
    }
    else {
        printf("Value not found for key: %c\n", key_to_find);
    }

    // 删除记录
    KeyType key_to_delete = 'b';
    if (HashDelete(&table, key_to_delete)) {
        printf("Record with key %c deleted successfully.\n", key_to_delete);
    }
    else {
        printf("Record with key %c not found.\n", key_to_delete);
    }

    // 销毁哈希表
    destroyHashTable(&table);

    return 0;
}
