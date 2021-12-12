#include "main.h"
# define MAX 99999 // 가장 큰 숫자(99999)를 MAX로 지정

// 트리 노드
typedef struct TreeNode {
    char ch; // 문자
    int weight; // 문자의 빈도수(누적한 값)
    struct TreeNode* left;
    struct TreeNode* right;
}TreeNode;

// heap의 element
typedef struct {
    TreeNode* htree; // hitten Tree
    char ch; //문자
    int key; // 문자의 빈도수(누적치) - 빈도수가 작은 것부터 합쳐진다
}element;

// heap
typedef struct {
    element heap[MAX];
    int heap_size; // 힙 사이즈
}HeapType;

// heap 초기화
HeapType* Heap_init() {
    HeapType* h = (HeapType*)malloc(sizeof(HeapType));
    h->heap_size = 0;
    return h;
}

// 힙 삽입 - 최소힙 : 빈도수가 적은 것들이 먼저 합쳐지기 때문에
void insert_min_heap(HeapType* h, element item) {
    int i;
    i = ++(h->heap_size);
    // 트리를 거슬러 올라가면서 부모 노드와 비교
    while ((i != 1) && (item.key < h->heap[i / 2].key)) {
        h->heap[i] = h->heap[i / 2]; //부모를 한 칸 내리기
        i /= 2;
    }
    h->heap[i] = item; // 새로운 노드 삽입
}

// 힙 삭제 
element delete_min_heap(HeapType* h) {
    int parent, child;
    element item, temp;
    item = h->heap[1]; // 루트 노드를 item으로 바꾸기
    temp = h->heap[(h->heap_size)--]; //마지막 노드를 temp로 옮기기
    parent = 1; child = 2;
    // 자식이 힙을 벗어나지 않았으면 반복문 진행
    while (child <= h->heap_size) {
        // 두 자식 노드 중 더 작은 자식노드 찾기
        if ((h->heap[parent * 2].key >= 0) && (h->heap[(parent * 2) + 1].key >= 0)) {
            if ((h->heap[parent * 2].key) < (h->heap[(parent * 2) + 1].key))
                child = parent * 2;
            else
                child = parent * 2 + 1;
        }
        if (temp.key <= h->heap[child].key) break;

        // 한 단계 아래로 이동
        h->heap[parent] = h->heap[child];
        parent = child;
        child *= 2;
    }
    h->heap[parent] = temp;
    return item;
}

// 히든 트리 생성 
TreeNode* make_HTree(TreeNode* left, TreeNode* right) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode)); // 공간할당
    node->left = left;
    node->right = right;
    return node;
}

// leaf node 판단 - leaf node면 1반환 아니면 0반환
int is_leaf(TreeNode* T) {
    return !(T->left) && !(T->right);
}

/*
typedef struct _Image_information
{
    FILE* stream; // bitstream 저장
    Int Best_Mode; // 가장 최적 예측모드
}Img_Info;

typedef struct _Image_Buffer
{
    Int* Quant_blk; // 양자화된 블록

    Int Huff_codes[16]; //
    Int Huff_freq[16]; // 허프만 발생 symbol 개수
    Int Huff_ch[16]; // 발생 symbol
    Int Huff_length[16];
    Int Huff_Size; // symbol 개수

    Img_Info info; // bitstream, best_mode 저장
}Img_Buf;
*/

// huffman
typedef struct {
    char ch; // 문자
    int freq; // 빈도수
    int* codes; // 코드
    int codes_stop; // 길이
}Huff;
Huff* huff;
int huff_code_idx = 0;

// huffman code 저장
void codes_array(int codes[], int length, int freq, int ch) {
    // static int i = 0;
    

    huff[huff_code_idx].ch = ch; // 문자
    huff[huff_code_idx].freq = freq; // 빈도수
    huff[huff_code_idx].codes_stop = length; // 길이

    // 코드 저장
    for (int k = 0; k < huff[huff_code_idx].codes_stop; k++)
        (huff[huff_code_idx].codes)[k] = codes[k];
    
    // 출력해보기
    printf("%d번째 문자 : %d, 빈도수 : %d, 길이 : %d, 코드 : ", huff_code_idx, huff[huff_code_idx].ch, huff[huff_code_idx].freq, huff[huff_code_idx].codes_stop);
    for (int k = 0; k < length; k++)
        printf("%d", huff[huff_code_idx].codes[k]);
    printf("\n");
    

    huff_code_idx++;
}

// 허프만 코드가 담긴 배열 생성 - 왼쪽 branch 1, 오른쪽 branch 0
void make_codes_array(TreeNode* T, int codes[], int idx) {
    // 왼쪽 자식이 있으면 1 저장, 순환 호출
    if (T->left) {
        codes[idx] = 1;
        make_codes_array(T->left, codes, idx + 1);
    }
    // 오른쪽 자식이 있으면 0저장, 순환 호출
    if (T->right) {
        codes[idx] = 0;
        make_codes_array(T->right, codes, idx + 1);
    }
    // leaf node면 허프만 코드 배열을 구조체에 저장
    if (is_leaf(T))
        codes_array(codes, idx, T->weight, T->ch);
}

// 허프만 힙을 이용하여 허프만 트리 생성
/*
허프만 힙 : 허프만 트리 구축을 위한 힙
히든 트리 : 허프만 힙의 원소가 갖는 숨겨진 트리
허프만 트리 : 허프만 코드 생성을 위한 트리
*/

TreeNode* huffman_tree; // 허프만트리

// make_huffman_heap : 발생symbol frequency 배열, 발생symbol배열, 발생symbol개수
void make_huffman_heap(int freq[], int ch_list[], int n) {
    int i;
    TreeNode* hnode; // 히든 트리 노드
    HeapType* h; // heap
    element e, e1, e2; // e:새로 삽입할 힙 원소, e1, e2:임의 힙 노드
    int codes[100];

    h = Heap_init(); // heap 초기화
    for (i = 0; i < n; i++) {
        hnode = make_HTree(NULL, NULL); // hiden tree node 생성
        e.ch = hnode->ch = ch_list[i];
        e.key = hnode->weight = freq[i];
        e.htree = hnode;
        insert_min_heap(h, e);
    }

    // 허프만 트리 구축
    for (i = 1; i < n; i++) {
        // 최소값 노드 2개 삭제
        e1 = delete_min_heap(h);
        e2 = delete_min_heap(h);
        // 최소값 노드 2개 합쳐서 히든 트리 생성
        hnode = make_HTree(e1.htree, e2.htree);
        // 허프만 힙에 새 원소 삽입 - 두개 key 더하기
        e.key = hnode->weight = e1.key + e2.key;
        e.htree = hnode;
        insert_min_heap(h, e);
    }
    e = delete_min_heap(h);
    huffman_tree = e.htree; // 최종 히든 트리(허프만트리)

    // 코드생성
    make_codes_array(huffman_tree, codes, 0);
}

Entropy(Int BLK, Img_Buf* img)
{
	Int ALL_Zero_flag = 0; //양자화된 변환 블록 내 화소값들이 전부 0인 경우
	UChar bitstream = 0;

    // 양자화된 block 4x4
    int blkSize = 4;
    int idx = 0; // 양자화블록 인덱스

    // 1) 처음 1byte 읽어오기
	fread(&bitstream, sizeof(UChar), 1, img->Input_Bitstream);
    printf("\n처음 1byte bitstream : %d\n", bitstream);
    if ((bitstream == 48) || (bitstream == 12) || (bitstream == 3) || (bitstream == 0)) {
        // 양자화된 블록의 모든 원소가 0이라는 것을 뜻함
        // 예측 정보 저장
        if (bitstream == 48) img->info.Best_Mode = 0;
        else if (bitstream == 12) img->info.Best_Mode = 1;
        else if (bitstream == 3) img->info.Best_Mode = 2;
        else if (bitstream == 0) img->info.Best_Mode = 3;

        // 양자화 블록에 0 채우기
        for (int i = 0; i < blkSize * blkSize; i++)
            img->Quant_blk[i] = 0;

        for (int i = 0; i < blkSize * blkSize; i++)
            printf("%d ", img->Quant_blk[i]);
        printf("\n");
    }
    else if ((bitstream == 240) || (bitstream == 204) || (bitstream == 195) || (bitstream == 192)) {
        // 양자화된 블록의 모든 원소가 0이 아니라는 것을 뜻함 - huffman decode 하기
        // 예측 정보 저장
        if (bitstream == 240) img->info.Best_Mode = 0;
        else if (bitstream == 204) img->info.Best_Mode = 1;
        else if (bitstream == 195) img->info.Best_Mode = 2;
        else if (bitstream == 192) img->info.Best_Mode = 3;

        // huffman decode 하기 위해 bitstream 가져오기
        // 2) 발생심볼수 - huff size
        fread(&bitstream, sizeof(UChar), 1, img->Input_Bitstream);
        img->Huff_Size = bitstream;
        // 3) 각 발생심볼 - huff ch
        for (int i = 0; i < img->Huff_Size; i++) {
            fread(&bitstream, sizeof(Char), 1, img->Input_Bitstream);
            img->Huff_ch[i] = bitstream;
        }
        // 4) 각 발생심볼의 개수 - huff freq
        for (int i = 0; i < img->Huff_Size; i++) {
            fread(&bitstream, sizeof(UChar), 1, img->Input_Bitstream);
            img->Huff_freq[i] = bitstream;
        }
        
        printf("발생심볼수 : %d, 각 발생심볼 : ", img->Huff_Size);
        for (int i = 0; i < img->Huff_Size; i++)
            printf("%d ", img->Huff_ch[i]);
        printf(", 각 발생심볼의 개수 : ");
        for (int i = 0; i < img->Huff_Size; i++)
            printf("%d ", img->Huff_freq[i]);
        printf("\n");
        


        // huffman 정보 저장 공간할당
        huff = (Huff*)malloc(sizeof(Huff) * MAX);
        for (int i = 0; i < img->Huff_Size; i++) {
            huff[i].codes = (int*)malloc(sizeof(Huff) * MAX);
        }

        // 구조체에 저장
        make_huffman_heap(img->Huff_freq, img->Huff_ch, img->Huff_Size);

        // 허프만 트리 decoding
        TreeNode* temp = huffman_tree;
        UChar bits;
        int k=0;
        fread(&bitstream, sizeof(UChar), 1, img->Input_Bitstream); // bitstream 열기
        printf("화소 bitstream : %d\n", bitstream);
        while (1) {
            bits = (bitstream & (1 << (7-k))) >> (7-k); // 2진수로 가져오기 - k번째 자리 저장
            printf("%d번째 bits : %d\n", k, bits);
            k += 1;
            if (bits == 1) {
                temp = temp->left;
                if ((temp->left == NULL) && (temp->right == NULL)) {
                    img->Quant_blk[idx] = temp->ch;
                    temp = huffman_tree; // 허프만트리의 root로
                    idx++;
                }
            }
            else {
                temp = temp->right;
                if ((temp->left == NULL) && (temp->right == NULL)) {
                    img->Quant_blk[idx] = temp->ch;
                    temp = huffman_tree; // 허프만트리의 root로
                    idx++;
                }
            }
            // 양자화된 블록이 다 채워진 경우
            if (idx == blkSize * blkSize)
                break;
            if (k == 8) {
                k = 0;
                fread(&bitstream, sizeof(UChar), 1, img->Input_Bitstream); // bitstream 열기
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
