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
    
    
    // ����غ���
    printf("%d��° ���� : %d, �󵵼� : %d, ���� : %d, �ڵ� : ", huff_code_idx, ch, freq, length);
    for (int k = 0; k < length; k++)
        printf("%d", codes[k]);
    printf("\n");
    


    huff[huff_code_idx].ch = ch; // ����
    huff[huff_code_idx].freq = freq; // �󵵼�
    huff[huff_code_idx].codes_stop = length; // ����

    // �ڵ� ����
    for (int k = 0; k < huff[huff_code_idx].codes_stop; k++)
        (huff[huff_code_idx].codes)[k] = codes[k];
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
/*
* int get_symbol(Img_Buf* img, int idx) {
    return img->Quant_blk[idx];
}
*/


Entropy(Int BLK, Img_Buf* img)
{
    // ����ȭ�� block 4x4
    int blkSize = 4;

    UChar code = 0; // 1byte ����
    int check_zero = 0; // block�� ��� 0����(0) �ƴ���(1) Ȯ��
    int isThere = 0; // �߻� symbol �ߺ����� �ʰ� Ȯ��
    int cnt = 0; // �߻� frequency ����
    img->Huff_Size = 0; // �߻��� symbol ���� 0���� �ʱ�ȭ

    
    printf("����ȭ�� ��ϰ� : ");
    for (int i = 0; i < blkSize * blkSize; i++)
        printf("%d ", img->Quant_blk[i]);
    printf("\n");
    

    for (int i = 0; i < blkSize * blkSize; i++) {
        // Huff_ch : �߻��� symbol ����
        // Huff_Size : �߻��� symbol ����
        isThere = 0;
        for (int j = 0; j < img->Huff_Size; j++) {
            if (img->Huff_ch[j] == img->Quant_blk[i]) {
                isThere = 1;
                break;
            }
        }
        // �߻��� symbol ���� ����
        if (isThere == 0) {
            img->Huff_ch[img->Huff_Size] = img->Quant_blk[i];
            img->Huff_Size++; 
        }
        // check_zero - block�� ��� 0���� Ȯ��
        if (img->Quant_blk[i] != 0)
            check_zero = 1;
    }
    // ó�� 1byte �־��ֱ�
    if (check_zero == 0) {
        if (img->info.Best_Mode == 0) code = 48;
        else if (img->info.Best_Mode == 1) code = 12;
        else if (img->info.Best_Mode == 2) code = 3;
        else if (img->info.Best_Mode == 3) code = 0;
    }
    else if (check_zero == 1) {
        if (img->info.Best_Mode == 0) code = 240;
        else if (img->info.Best_Mode == 1) code = 204;
        else if (img->info.Best_Mode == 2) code = 195;
        else if (img->info.Best_Mode == 3) code = 192;
    }
    // 240, 204, 195, 192, 48, 12, 3, 0

    printf("check zero : %d, Best mode : %d\n", check_zero, img->info.Best_Mode);
    printf("ó�� 1byte stream : %d\n", code);
    fwrite(&code, sizeof(UChar), 1, img->info.stream); // �ڵ忡 ����

    // ����ȭ�� ����� ���� ��� 0�� �ƴ� ��� - huffman coding ����
    if (check_zero == 1) {
        // Huff_freq : symbol�߻� frequency ����
        for (int i = 0; i < img->Huff_Size; i++) {
            cnt = 0;
            for (int j = 0; j < blkSize * blkSize; j++) {
                if (img->Quant_blk[j] == img->Huff_ch[i]) cnt++;
            }
            img->Huff_freq[i] = cnt;
        }

        
        printf("huffman size : %d\n", img->Huff_Size);
        for (int i = 0; i < img->Huff_Size; i++)
            printf("huffman symbol : %d, huffman frequency: %d\n", img->Huff_ch[i], img->Huff_freq[i]);
        //printf("\n");
        

        // 2) �߻��ɺ��� Huffman Size
        fwrite(&img->Huff_Size, sizeof(UChar), 1, img->info.stream); // �ڵ忡 ����
        // 3) �� �߻��ɺ� - huff_ch
        for (int i = 0; i < img->Huff_Size; i++)
            fwrite(&img->Huff_ch[i], sizeof(Char), 1, img->info.stream); // �ڵ忡 ����(�����ϼ��� �ֱ� ������ char������ �����ֱ�)
        // 4)�� �߻��ɺ��� ���� ���� - huff_freq
        for (int i = 0; i < img->Huff_Size; i++)
            fwrite(&img->Huff_freq[i], sizeof(UChar), 1, img->info.stream); // �ڵ忡 ����

        // huffman ���� ���� �����Ҵ�
        huff = (Huff*)malloc(sizeof(Huff) * MAX);
        for (int i = 0; i < img->Huff_Size; i++) {
            huff[i].codes = (int*)malloc(sizeof(Huff) * MAX);
        }
        
        // ����ü�� ���� - ������ �ڵ� �����
        make_huffman_heap(img->Huff_freq, img->Huff_ch, img->Huff_Size);

        // ������ �ڵ���� �Է��� ���ڿ� �°� �����ϱ�
        int tmp, aa =0;
        cnt = 0;
        code = 0; // encode�Ǵ� bitstream
        for(int l = 0 ; l < blkSize * blkSize ; l++) {
            tmp = img->Quant_blk[l]; // ����ȭ�� ��Ͽ��� ȭ�Ұ� �ϳ��� ������
            // ȭ�Ұ��� �ش��ϴ� ������ �ڵ� ����
            for (int i = 0; i < img->Huff_Size; i++) {
                if (huff[i].ch == tmp) {
                    // code�� �ش��ϴ� ������ �ڵ� �Է�
                    for (int k = 0; k < huff[i].codes_stop; k++) {
                        code <<= 1; cnt++; // bitstream�� 8�����ϱ� ����
                        if (((huff[i].codes)[k]) == 1) 
                            code |= 1;
                        // bitstream�� �� ����
                        if (cnt == 8) {
                            printf("%d��° bitstream : %d\n", aa, code); aa ++;
                            fwrite(&code, sizeof(UChar), 1, img->info.stream); // �ڵ忡 ����
                            code = 0; cnt = 0; // �ʱ�ȭ
                        }
                    }
                    break;
                }
            }
            // bit�� �������� ���
            if ((l == blkSize*blkSize -1) && (cnt != 0)) {
                code = code << (8 - cnt);
                printf("%d��° bitstream : %d\n", aa, code); aa ++;
                fwrite(&code, sizeof(UChar), 1, img->info.stream); // �ڵ忡 ����
                break;
            }
        }
        printf("\n");
        huff_code_idx = 0;
        
        for (int i = 0; i < img->Huff_Size; i++) {
            free(huff[i].codes);
        }
        free(huff);
    }

}