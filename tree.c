#include <stdio.h>
#include <stdlib.h>

typedef enum {
    CHAPTER,
    SECTION,
    SUBSECTION,
    SUBSUBSECTION
} NodeType;

typedef struct Node {
    NodeType type;
    char title[100];
    int pages;
    struct Node *children;
    struct Node *next;
} Node;

void my_strcpy(char *dest, const char *src) {
    int i = 0;
    while (src[i] != '\0' && i < 99) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

int my_strcmp(const char *s1, const char *s2) {
    int i = 0;
    while (s1[i] != '\0' && s2[i] != '\0') {
        if (s1[i] < s2[i]) return -1;
        if (s1[i] > s2[i]) return 1;
        i++;
    }
    if (s1[i] == '\0' && s2[i] == '\0') return 0;
    if (s1[i] == '\0') return -1;
    return 1;
}

Node* createNode(NodeType type, const char* title, int pages) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->type = type;
    my_strcpy(node->title, title);
    node->pages = pages;
    node->children = NULL;
    node->next = NULL;
    return node;
}

void addChild(Node* parent, Node* child) {
    if (parent == NULL || child == NULL) return;
    child->next = parent->children;
    parent->children = child;
}

int totalPages(Node* node) {
    if (node == NULL) return 0;
    int sum = node->pages;
    Node* c = node->children;
    while (c != NULL) {
        sum += totalPages(c);
        c = c->next;
    }
    return sum;
}

int countChapters(Node* root) {
    if (root == NULL) return 0;
    int count = (root->type == CHAPTER) ? 1 : 0;
    Node* c = root->children;
    while (c != NULL) {
        count += countChapters(c);
        c = c->next;
    }
    return count;
}

void findLongestChapter(Node* root, Node** longestChapter, int* maxPages) {
    if (root == NULL) return;
    if (root->type == CHAPTER) {
        int tp = totalPages(root);
        if (tp > *maxPages) {
            *maxPages = tp;
            *longestChapter = root;
        }
    }
    Node* c = root->children;
    while (c != NULL) {
        findLongestChapter(c, longestChapter, maxPages);
        c = c->next;
    }
}

void freeSubtree(Node* n) {
    if (n == NULL) return;
    Node* c = n->children;
    while (c) {
        Node* tmp = c->next;
        freeSubtree(c);
        c = tmp;
    }
    free(n);
}

int deleteNodeByTitle(Node* parent, const char* title) {
    if (parent == NULL) return 0;
    Node* prev = NULL;
    Node* curr = parent->children;
    while (curr != NULL) {
        if (my_strcmp(curr->title, title) == 0) {
            if (prev == NULL) {
                parent->children = curr->next;
            } else {
                prev->next = curr->next;
            }
            freeSubtree(curr);
            return 1;
        }
        prev = curr;
        curr = curr->next;
    }

    Node* c = parent->children;
    while (c != NULL) {
        if (deleteNodeByTitle(c, title)) {
            return 1;
        }
        c = c->next;
    }
    return 0;
}

void printTree(Node* root, int level) {
    if (root == NULL) return;
    int i;
    for (i = 0; i < level; i++) printf("  ");
    const char* typeStr = (root->type == CHAPTER) ? "CHAPTER" :
                          (root->type == SECTION) ? "SECTION" :
                          (root->type == SUBSECTION) ? "SUBSECTION" : "SUBSUBSECTION";
    printf("%s: %s (pages=%d, total=%d)\n", typeStr, root->title, root->pages, totalPages(root));
    Node* c = root->children;
    while (c != NULL) {
        printTree(c, level + 1);
        c = c->next;
    }
}

int main() {
    Node* book = createNode(CHAPTER, "Chuong 1", 10);
    Node* sec1 = createNode(SECTION, "Muc 1.1", 5);
    Node* sec2 = createNode(SECTION, "Muc 1.2", 3);
    Node* subsec1 = createNode(SUBSECTION, "Tieu muc 1.2.1", 2);
    addChild(book, sec2);
    addChild(book, sec1);
    addChild(sec2, subsec1);

    Node* ch2 = createNode(CHAPTER, "Chuong 2", 20);
    Node* sec3 = createNode(SECTION, "Muc 2.1", 10);
    Node* subsub1 = createNode(SUBSUBSECTION, "Muc 2.1.1.1", 1);
    addChild(ch2, sec3);
    addChild(sec3, subsub1);

    // Đổi loại của DummyRoot thành SECTION để không bị tính là CHAPTER
    Node* root = createNode(SECTION, "DummyRoot", 0);
    addChild(root, ch2);
    addChild(root, book);

    printf("Cau truc ban dau:\n");
    printTree(root, 0);

    printf("\nSo chuong cua cuon sach: %d\n", countChapters(root));

    Node* longest = NULL;
    int maxPages = 0;
    findLongestChapter(root, &longest, &maxPages);
    if (longest != NULL) {
        printf("Chuong dai nhat: %s voi %d trang (tinh ca con chau)\n", longest->title, maxPages);
    }

    printf("\nXoa muc 'Muc 1.2'...\n");
    if (deleteNodeByTitle(root, "Muc 1.2")) {
        printf("Da xoa thanh cong.\n");
    } else {
        printf("Khong tim thay muc can xoa.\n");
    }

    printf("\nCau truc sau khi xoa:\n");
    printTree(root, 0);

    freeSubtree(root);

    return 0;
}
    