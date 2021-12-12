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
    
    
    // 출력해보기
    printf("%d번째 문자 : %d, 빈도수 : %d, 길이 : %d, 코드 : ", huff_code_idx, ch, freq, length);
    for (int k = 0; k < length; k++)
        printf("%d", codes[k]);
    printf("\n");
    


    huff[huff_code_idx].ch = ch; // 문자
    huff[huff_code_idx].freq = freq; // 빈도수
    huff[huff_code_idx].codes_stop = length; // 길이

    // 코드 저장
    for (int k = 0; k < huff[huff_code_idx].codes_stop; k++)
        (huff[huff_code_idx].codes)[k] = codes[k];
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
/*
* int get_symbol(Img_Buf* img, int idx) {
    return img->Quant_blk[idx];
}
*/


Entropy(Int BLK, Img_Buf* img)
{
    // 양자화된 block 4x4
    int blkSize = 4;

    UChar code = 0; // 1byte 저장
    int check_zero = 0; // block이 모두 0인지(0) 아닌지(1) 확인
    int isThere = 0; // 발생 symbol 중복되지 않게 확인
    int cnt = 0; // 발생 frequency 세기
    img->Huff_Size = 0; // 발생한 symbol 개수 0으로 초기화

    
    printf("양자화된 블록값 : ");
    for (int i = 0; i < blkSize * blkSize; i++)
        printf("%d ", img->Quant_blk[i]);
    printf("\n");
    

    for (int i = 0; i < blkSize * blkSize; i++) {
        // Huff_ch : 발생한 symbol 종류
        // Huff_Size : 발생한 symbol 개수
        isThere = 0;
        for (int j = 0; j < img->Huff_Size; j++) {
            if (img->Huff_ch[j] == img->Quant_blk[i]) {
                isThere = 1;
                break;
            }
        }
        // 발생한 symbol 종류 개수
        if (isThere == 0) {
            img->Huff_ch[img->Huff_Size] = img->Quant_blk[i];
            img->Huff_Size++; 
        }
        // check_zero - block이 모두 0인지 확인
        if (img->Quant_blk[i] != 0)
            check_zero = 1;
    }
    // 처음 1byte 넣어주기
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
    printf("처음 1byte stream : %d\n", code);
    fwrite(&code, sizeof(UChar), 1, img->info.stream); // 코드에 쓰기

    // 양자화된 블록의 값이 모두 0이 아닌 경우 - huffman coding 진행
    if (check_zero == 1) {
        // Huff_freq : symbol발생 frequency 세기
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
        

        // 2) 발생심볼수 Huffman Size
        fwrite(&img->Huff_Size, sizeof(UChar), 1, img->info.stream); // 코드에 쓰기
        // 3) 각 발생심볼 - huff_ch
        for (int i = 0; i < img->Huff_Size; i++)
            fwrite(&img->Huff_ch[i], sizeof(Char), 1, img->info.stream); // 코드에 쓰기(음수일수도 있기 때문에 char형으로 보내주기)
        // 4)각 발생심볼의 개수 전송 - huff_freq
        for (int i = 0; i < img->Huff_Size; i++)
            fwrite(&img->Huff_freq[i], sizeof(UChar), 1, img->info.stream); // 코드에 쓰기

        // huffman 정보 저장 공간할당
        huff = (Huff*)malloc(sizeof(Huff) * MAX);
        for (int i = 0; i < img->Huff_Size; i++) {
            huff[i].codes = (int*)malloc(sizeof(Huff) * MAX);
        }
        
        // 구조체에 저장 - 허프만 코드 만들기
        make_huffman_heap(img->Huff_freq, img->Huff_ch, img->Huff_Size);

        // 허프만 코드들을 입력한 문자에 맞게 저장하기
        int tmp, aa =0;
        cnt = 0;
        code = 0; // encode되는 bitstream
        for(int l = 0 ; l < blkSize * blkSize ; l++) {
            tmp = img->Quant_blk[l]; // 양자화된 블록에서 화소값 하나씩 얻어오기
            // 화소값에 해당하는 허프만 코드 저장
            for (int i = 0; i < img->Huff_Size; i++) {
                if (huff[i].ch == tmp) {
                    // code에 해당하는 허프만 코드 입력
                    for (int k = 0; k < huff[i].codes_stop; k++) {
                        code <<= 1; cnt++; // bitstream이 8까지니까 세기
                        if (((huff[i].codes)[k]) == 1) 
                            code |= 1;
                        // bitstream이 다 차면
                        if (cnt == 8) {
                            printf("%d번째 bitstream : %d\n", aa, code); aa ++;
                            fwrite(&code, sizeof(UChar), 1, img->info.stream); // 코드에 쓰기
                            code = 0; cnt = 0; // 초기화
                        }
                    }
                    break;
                }
            }
            // bit가 남아있을 경우
            if ((l == blkSize*blkSize -1) && (cnt != 0)) {
                code = code << (8 - cnt);
                printf("%d번째 bitstream : %d\n", aa, code); aa ++;
                fwrite(&code, sizeof(UChar), 1, img->info.stream); // 코드에 쓰기
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