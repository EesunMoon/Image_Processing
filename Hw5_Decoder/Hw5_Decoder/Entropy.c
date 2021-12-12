#include "main.h"
# define MAX 99999 // ���� ū ����(99999)�� MAX�� ����

// Ʈ�� ���
typedef struct TreeNode {
    char ch; // ����
    int weight; // ������ �󵵼�(������ ��)
    struct TreeNode* left;
    struct TreeNode* right;
}TreeNode;

// heap�� element
typedef struct {
    TreeNode* htree; // hitten Tree
    char ch; //����
    int key; // ������ �󵵼�(����ġ) - �󵵼��� ���� �ͺ��� ��������
}element;

// heap
typedef struct {
    element heap[MAX];
    int heap_size; // �� ������
}HeapType;

// heap �ʱ�ȭ
HeapType* Heap_init() {
    HeapType* h = (HeapType*)malloc(sizeof(HeapType));
    h->heap_size = 0;
    return h;
}

// �� ���� - �ּ��� : �󵵼��� ���� �͵��� ���� �������� ������
void insert_min_heap(HeapType* h, element item) {
    int i;
    i = ++(h->heap_size);
    // Ʈ���� �Ž��� �ö󰡸鼭 �θ� ���� ��
    while ((i != 1) && (item.key < h->heap[i / 2].key)) {
        h->heap[i] = h->heap[i / 2]; //�θ� �� ĭ ������
        i /= 2;
    }
    h->heap[i] = item; // ���ο� ��� ����
}

// �� ���� 
element delete_min_heap(HeapType* h) {
    int parent, child;
    element item, temp;
    item = h->heap[1]; // ��Ʈ ��带 item���� �ٲٱ�
    temp = h->heap[(h->heap_size)--]; //������ ��带 temp�� �ű��
    parent = 1; child = 2;
    // �ڽ��� ���� ����� �ʾ����� �ݺ��� ����
    while (child <= h->heap_size) {
        // �� �ڽ� ��� �� �� ���� �ڽĳ�� ã��
        if ((h->heap[parent * 2].key >= 0) && (h->heap[(parent * 2) + 1].key >= 0)) {
            if ((h->heap[parent * 2].key) < (h->heap[(parent * 2) + 1].key))
                child = parent * 2;
            else
                child = parent * 2 + 1;
        }
        if (temp.key <= h->heap[child].key) break;

        // �� �ܰ� �Ʒ��� �̵�
        h->heap[parent] = h->heap[child];
        parent = child;
        child *= 2;
    }
    h->heap[parent] = temp;
    return item;
}

// ���� Ʈ�� ���� 
TreeNode* make_HTree(TreeNode* left, TreeNode* right) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode)); // �����Ҵ�
    node->left = left;
    node->right = right;
    return node;
}

// leaf node �Ǵ� - leaf node�� 1��ȯ �ƴϸ� 0��ȯ
int is_leaf(TreeNode* T) {
    return !(T->left) && !(T->right);
}

/*
typedef struct _Image_information
{
    FILE* stream; // bitstream ����
    Int Best_Mode; // ���� ���� �������
}Img_Info;

typedef struct _Image_Buffer
{
    Int* Quant_blk; // ����ȭ�� ���

    Int Huff_codes[16]; //
    Int Huff_freq[16]; // ������ �߻� symbol ����
    Int Huff_ch[16]; // �߻� symbol
    Int Huff_length[16];
    Int Huff_Size; // symbol ����

    Img_Info info; // bitstream, best_mode ����
}Img_Buf;
*/

// huffman
typedef struct {
    char ch; // ����
    int freq; // �󵵼�
    int* codes; // �ڵ�
    int codes_stop; // ����
}Huff;
Huff* huff;
int huff_code_idx = 0;

// huffman code ����
void codes_array(int codes[], int length, int freq, int ch) {
    // static int i = 0;
    

    huff[huff_code_idx].ch = ch; // ����
    huff[huff_code_idx].freq = freq; // �󵵼�
    huff[huff_code_idx].codes_stop = length; // ����

    // �ڵ� ����
    for (int k = 0; k < huff[huff_code_idx].codes_stop; k++)
        (huff[huff_code_idx].codes)[k] = codes[k];
    
    // ����غ���
    printf("%d��° ���� : %d, �󵵼� : %d, ���� : %d, �ڵ� : ", huff_code_idx, huff[huff_code_idx].ch, huff[huff_code_idx].freq, huff[huff_code_idx].codes_stop);
    for (int k = 0; k < length; k++)
        printf("%d", huff[huff_code_idx].codes[k]);
    printf("\n");
    

    huff_code_idx++;
}

// ������ �ڵ尡 ��� �迭 ���� - ���� branch 1, ������ branch 0
void make_codes_array(TreeNode* T, int codes[], int idx) {
    // ���� �ڽ��� ������ 1 ����, ��ȯ ȣ��
    if (T->left) {
        codes[idx] = 1;
        make_codes_array(T->left, codes, idx + 1);
    }
    // ������ �ڽ��� ������ 0����, ��ȯ ȣ��
    if (T->right) {
        codes[idx] = 0;
        make_codes_array(T->right, codes, idx + 1);
    }
    // leaf node�� ������ �ڵ� �迭�� ����ü�� ����
    if (is_leaf(T))
        codes_array(codes, idx, T->weight, T->ch);
}

// ������ ���� �̿��Ͽ� ������ Ʈ�� ����
/*
������ �� : ������ Ʈ�� ������ ���� ��
���� Ʈ�� : ������ ���� ���Ұ� ���� ������ Ʈ��
������ Ʈ�� : ������ �ڵ� ������ ���� Ʈ��
*/

TreeNode* huffman_tree; // ������Ʈ��

// make_huffman_heap : �߻�symbol frequency �迭, �߻�symbol�迭, �߻�symbol����
void make_huffman_heap(int freq[], int ch_list[], int n) {
    int i;
    TreeNode* hnode; // ���� Ʈ�� ���
    HeapType* h; // heap
    element e, e1, e2; // e:���� ������ �� ����, e1, e2:���� �� ���
    int codes[100];

    h = Heap_init(); // heap �ʱ�ȭ
    for (i = 0; i < n; i++) {
        hnode = make_HTree(NULL, NULL); // hiden tree node ����
        e.ch = hnode->ch = ch_list[i];
        e.key = hnode->weight = freq[i];
        e.htree = hnode;
        insert_min_heap(h, e);
    }

    // ������ Ʈ�� ����
    for (i = 1; i < n; i++) {
        // �ּҰ� ��� 2�� ����
        e1 = delete_min_heap(h);
        e2 = delete_min_heap(h);
        // �ּҰ� ��� 2�� ���ļ� ���� Ʈ�� ����
        hnode = make_HTree(e1.htree, e2.htree);
        // ������ ���� �� ���� ���� - �ΰ� key ���ϱ�
        e.key = hnode->weight = e1.key + e2.key;
        e.htree = hnode;
        insert_min_heap(h, e);
    }
    e = delete_min_heap(h);
    huffman_tree = e.htree; // ���� ���� Ʈ��(������Ʈ��)

    // �ڵ����
    make_codes_array(huffman_tree, codes, 0);
}

Entropy(Int BLK, Img_Buf* img)
{
	Int ALL_Zero_flag = 0; //����ȭ�� ��ȯ ��� �� ȭ�Ұ����� ���� 0�� ���
	UChar bitstream = 0;

    // ����ȭ�� block 4x4
    int blkSize = 4;
    int idx = 0; // ����ȭ��� �ε���

    // 1) ó�� 1byte �о����
	fread(&bitstream, sizeof(UChar), 1, img->Input_Bitstream);
    printf("\nó�� 1byte bitstream : %d\n", bitstream);
    if ((bitstream == 48) || (bitstream == 12) || (bitstream == 3) || (bitstream == 0)) {
        // ����ȭ�� ����� ��� ���Ұ� 0�̶�� ���� ����
        // ���� ���� ����
        if (bitstream == 48) img->info.Best_Mode = 0;
        else if (bitstream == 12) img->info.Best_Mode = 1;
        else if (bitstream == 3) img->info.Best_Mode = 2;
        else if (bitstream == 0) img->info.Best_Mode = 3;

        // ����ȭ ��Ͽ� 0 ä���
        for (int i = 0; i < blkSize * blkSize; i++)
            img->Quant_blk[i] = 0;

        for (int i = 0; i < blkSize * blkSize; i++)
            printf("%d ", img->Quant_blk[i]);
        printf("\n");
    }
    else if ((bitstream == 240) || (bitstream == 204) || (bitstream == 195) || (bitstream == 192)) {
        // ����ȭ�� ����� ��� ���Ұ� 0�� �ƴ϶�� ���� ���� - huffman decode �ϱ�
        // ���� ���� ����
        if (bitstream == 240) img->info.Best_Mode = 0;
        else if (bitstream == 204) img->info.Best_Mode = 1;
        else if (bitstream == 195) img->info.Best_Mode = 2;
        else if (bitstream == 192) img->info.Best_Mode = 3;

        // huffman decode �ϱ� ���� bitstream ��������
        // 2) �߻��ɺ��� - huff size
        fread(&bitstream, sizeof(UChar), 1, img->Input_Bitstream);
        img->Huff_Size = bitstream;
        // 3) �� �߻��ɺ� - huff ch
        for (int i = 0; i < img->Huff_Size; i++) {
            fread(&bitstream, sizeof(Char), 1, img->Input_Bitstream);
            img->Huff_ch[i] = bitstream;
        }
        // 4) �� �߻��ɺ��� ���� - huff freq
        for (int i = 0; i < img->Huff_Size; i++) {
            fread(&bitstream, sizeof(UChar), 1, img->Input_Bitstream);
            img->Huff_freq[i] = bitstream;
        }
        
        printf("�߻��ɺ��� : %d, �� �߻��ɺ� : ", img->Huff_Size);
        for (int i = 0; i < img->Huff_Size; i++)
            printf("%d ", img->Huff_ch[i]);
        printf(", �� �߻��ɺ��� ���� : ");
        for (int i = 0; i < img->Huff_Size; i++)
            printf("%d ", img->Huff_freq[i]);
        printf("\n");
        


        // huffman ���� ���� �����Ҵ�
        huff = (Huff*)malloc(sizeof(Huff) * MAX);
        for (int i = 0; i < img->Huff_Size; i++) {
            huff[i].codes = (int*)malloc(sizeof(Huff) * MAX);
        }

        // ����ü�� ����
        make_huffman_heap(img->Huff_freq, img->Huff_ch, img->Huff_Size);

        // ������ Ʈ�� decoding
        TreeNode* temp = huffman_tree;
        UChar bits;
        int k=0;
        fread(&bitstream, sizeof(UChar), 1, img->Input_Bitstream); // bitstream ����
        printf("ȭ�� bitstream : %d\n", bitstream);
        while (1) {
            bits = (bitstream & (1 << (7-k))) >> (7-k); // 2������ �������� - k��° �ڸ� ����
            printf("%d��° bits : %d\n", k, bits);
            k += 1;
            if (bits == 1) {
                temp = temp->left;
                if ((temp->left == NULL) && (temp->right == NULL)) {
                    img->Quant_blk[idx] = temp->ch;
                    temp = huffman_tree; // ������Ʈ���� root��
                    idx++;
                }
            }
            else {
                temp = temp->right;
                if ((temp->left == NULL) && (temp->right == NULL)) {
                    img->Quant_blk[idx] = temp->ch;
                    temp = huffman_tree; // ������Ʈ���� root��
                    idx++;
                }
            }
            // ����ȭ�� ����� �� ä���� ���
            if (idx == blkSize * blkSize)
                break;
            if (k == 8) {
                k = 0;
                fread(&bitstream, sizeof(UChar), 1, img->Input_Bitstream); // bitstream ����
                printf("bitstream : %d\n", bitstream);
            }
        }
        
        for (int i = 0; i < blkSize * blkSize; i++)
            printf("%d ", img->Quant_blk[i]);
        printf("\n");
        
        huff_code_idx = 0;
        
        for (int i = 0; i < img->Huff_Size; i++) {
            free(huff[i].codes);
        }
        free(huff); 
    }
    else {
    printf(" \n\n error!!!!!! \n\n");
    }
}
