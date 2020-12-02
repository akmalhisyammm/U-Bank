#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#define MAX 256
#define HashT_MAX 10
#define adminPin 123456

const char *filename = "accounts.txt";
const char *tempFilename = "accounts_tmp.txt";

FILE *ftemp;
FILE *fptr;
FILE *fhistory;

/*********************** CONSOLE FUNCTIONS ***********************/
int readLine(char *out){
	int len = 0;
	if (fgets(out, MAX, stdin) == NULL) return 0;
	len = strlen(out);
	if (len > 0) out[--len] = '\0';
	return len;
}

int readInt(){
	char temp[MAX];
	int input;
	readLine(temp);
	sscanf(temp, "%d", &input);
	return input;
}

int askInt(){
	int input = 0;
	input = readInt();
	return input;
}

void tolower_str(char *str){
    for(int i = 0; str[i]; i++){
        str[i] = tolower(str[i]);
    }
}

void pauseKey(){
    printf("\nPress ENTER to continue . . . ");
    getchar();
}

void clearScreen(){
    #if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
        system("clear");
    #endif

    #if defined(_WIN32) || defined(_WIN64)
        system("cls");
    #endif
}


/*********************** NODE STRUCTURE ***********************/
typedef struct _user{
    int kartu;
	int rekening;
	int saldo;
	int status;
	char nama[MAX];
	char pin[7];
	int treeKey;
	struct _user *next, *prev;
} User;

User *head, *tail, *curr, *node, *tempUser, *tempDirect, *temp_a, *temp_b;


/*********************** NODE FUNCTIONS ***********************/
void createNode(int rekening, int saldo, char nama[], char pin[], int kartu, int status){ //function untuk membuat node baru di linked list
	node = (User*) malloc(sizeof(User));
	node->next = NULL;
	node->prev = NULL;

	node->kartu = kartu;
	node->rekening = rekening;
	node->saldo = saldo;
	node->status = status;
	strcpy(node->nama, nama);
	strcpy(node->pin, pin);

	if(head == NULL){
		head = node;
		head->next = node;
		head->prev = node;
		tail = node;
	}
	else{
		tail->next = node;
		node->prev = tail;
		tail = node;
		tail->next = head;
		head->prev = tail;
	}
}


/*********************** QUEUE STRUCTURE ***********************/
typedef struct _queue{
    int kartu;
	int rekening;
	int saldo;
	int status;
	char nama[MAX];
	char pin[7];
	struct _queue *next;
} Queue;

Queue *qnode, *qhead, *qtail;


/*********************** QUEUE FUNCTIONS ***********************/
void enqueue(int rekening, int saldo, char nama[], char pin[], int kartu, int status){
    qnode = (Queue*) malloc(sizeof(Queue));

    qnode->kartu = kartu;
	qnode->rekening = rekening;
	qnode->saldo = saldo;
	qnode->status = status;
	strcpy(qnode->nama, nama);
	strcpy(qnode->pin, pin);

    qnode->next = NULL;

    if(qhead == NULL) qhead = qnode;
    else qtail->next = qnode;

    qtail = qnode;
}

void dequeue(){
    Queue *hapus = qhead;
    qhead = qhead->next;
    free(hapus);
}

int isEmpty(Queue *temphead){
    if(temphead == NULL) return 1;
    else return 0;
}


/*********************** HASH STRUCTURE ***************************/
typedef struct _hashing{
    int kartu;
    struct _hashing *next;
} Hashing;

Hashing *head_hash, *HashT[HashT_MAX];


/*********************** HASH FUNCTIONS ***************************/
void insertToChain(int key, Hashing **head){
    Hashing *ptr = (*head);
    Hashing *node = (Hashing*) malloc(sizeof(Hashing));

    node->kartu = key;
    node->next = NULL;

    if(*head == NULL){
        *head = node;
        head_hash = node;
    } else {
        while(ptr->next != NULL){
            ptr = ptr->next;
        }
        ptr->next = node;
    }
}


/*********************** TREE STRUCTURE ***************************/
typedef struct _tree{
    int key;
    struct _tree *left, *right;
} Tree;


/*********************** TREE FUNCTIONS ***************************/
Tree *newNode(int item){ //function untuk membuat node baru pada BST
    Tree *temp = (Tree*) malloc(sizeof(Tree));
    temp->key = item;
    temp->left = temp->right = NULL;
    return temp;
}

Tree *insert(Tree *node, int key){
    /* Jika tree null, dibuat nodenya */
    if(node == NULL) return newNode(key);

    /* Jika tidak null, maka akan turun ke bawah */
    if(key < node->key)
        node->left = insert(node->left, key);
    else if(key > node->key)
        node->right = insert(node->right, key);

    /* return node jika sudah selesai */
    return node;
}

void printTree(int key){
    curr = head;
    while(1){
        if(key == curr->kartu){
			printf("Nama Nasabah   : %s\n", curr->nama);
			printf("Nomor Kartu    : %d\n", curr->kartu);
			printf("Nomor Rekening : %d\n", curr->rekening);
			printf("PIN            : %s\n", curr->pin);
			printf("Saldo          : Rp%d\n", curr->saldo);
			printf("Status         : %s\n", curr->status ? "Terblokir" : "Normal"); //Untuk melihat apakah data pengguna sudah terblokir atau belum
			printf("------------------------------------\n");
        }
        curr = curr->next;
        if(curr == head)
            break;
    }
}

void preorder(Tree *pnode){
    if(pnode == NULL) return;
    printTree(pnode->key);
    preorder(pnode->left);
    preorder(pnode->right);
}

void inorder(Tree *pnode){
    if(pnode == NULL) return;
    inorder(pnode->left);
    printTree(pnode->key);
    inorder(pnode->right);
}

void postorder(Tree *pnode){
    if(pnode == NULL) return;
    postorder(pnode->left);
    postorder(pnode->right);
    printTree(pnode->key);
}

void replaceData(){ //function untuk mereplace data baru pengguna di accounts.txt
    ftemp = fopen(tempFilename, "w");

    curr = head;
    while(1){
        fprintf(ftemp, "nama=%s#kartu=%d#rekening=%d#pin=%s#saldo=%d#blocked=%d\n", curr->nama, curr->kartu, curr->rekening, curr->pin, curr->saldo, curr->status);
        curr = curr->next;
        if(curr == head)
            break;
    }

    fclose(ftemp);
    remove(filename);
    rename(tempFilename, filename);
}

void moveData(){ //function untuk memindahkan data dari accounts.txt ke linked list
    int rekening = 0, saldo = 0, kartu = 0, status = 0;
	char nama[MAX], pin[7];

	for(int i = 0; i < HashT_MAX; i++){
        HashT[i] = NULL; // Inisialisasi HashTable
    }

	fptr = fopen(filename, "r");
    while(!feof(fptr)){
        fscanf(fptr, "nama=%[^#]#kartu=%d#rekening=%d#pin=%[^#]#saldo=%d#blocked=%d ", nama, &kartu, &rekening, pin, &saldo, &status);

        if(rekening != 0){
            createNode(rekening, saldo, nama, pin, kartu, status);
            insertToChain(kartu, &HashT[kartu%10]);
        }
    }
    fclose(fptr);
}

int duplicateNumber(int choice, int temp){ //function untuk mengembalikan nilai integer dari nomor kartu / nomor rekening yang ada di file accounts.txt sesuai dengan parameter choice
    int rekening = 0, saldo = 0, kartu = 0, status = 0;
	char nama[MAX], pin[7];

    fptr = fopen(filename, "r");

    while(!feof(fptr)){
        fscanf(fptr, "nama=%[^#]#kartu=%d#rekening=%d#pin=%[^#]#saldo=%d#blocked=%d ", nama, &kartu, &rekening, pin, &saldo, &status);

		if(choice == 0 && kartu == temp){
			fclose(fptr);
			return kartu;
		}
		else if(choice == 1 && rekening == temp){
			fclose(fptr);
			return rekening;
		}
    }
	fclose(fptr);
	return 0;
}

int randomNumber(char request[]){ //function mendapatkan nomor kartu dan nomor rekening acak ketika pengguna mendaftar ke bank
    int result = 0, lower, upper;
    int temp = 0;

    do{
        temp = 0;
        srand(time(NULL));

        if(strcmp(request, "card") == 0){ //Jika string compare me-return value '0' alias true yang artinya parameter string "request" yang diterima sama dengan string pembanding
            lower = 10000; //Menentukan batas bawah bilangan
            upper = 99999; //Menentukan batas atas bilangan
            result = (rand() % (upper - lower + 1)) + lower;
            temp = duplicateNumber(0, result);
        }
        else if (strcmp(request, "rekening") == 0){
            lower = 100000000; //Menentukan batas bawah bilangan
            upper = 999999999; //Menentukan batas atas bilangan
            result = (rand() % (upper - lower + 1)) + lower;
            temp = duplicateNumber(1, result);
        }
    }while(temp == result);

	return result; //Kemudian return hasil dari operasi yang ada di variable 'result'
}

//function untuk mencatat setiap transaksi yang nasabah lakukan
void writeHistory(int rekening_tmp, int saldo_tmp, int tipe){
    fhistory = fopen(tempUser->nama, "a");

    fprintf(fhistory, "rekening=%d#nominal=%d#tipe=%d\n", rekening_tmp, saldo_tmp, tipe);

    fclose(fhistory);
}

//function untuk membuat file kosong dengan nama nasabah
void newFile(char *name){
	if(access(name, F_OK) != 0){
		fhistory = fopen(name, "a");
		fclose(fhistory);
	}
}

void infoMenu(){
	int pilihan = 0;
	int rekening_tmp = 0, saldo_tmp = 0, tipe = 0, i = 1;

	do{
    	clearScreen();
    	printf("===================================\n");
		printf("          Menu Info Saldo          \n");
		printf("===================================\n");
    	printf("1. Info Saldo Anda\n");
    	printf("2. Mutasi Rekening\n");
    	printf("0. Kembali\n");
    	printf("Pilihan : ");
    	pilihan = askInt();

    	if(pilihan >= 1 && pilihan <= 2){
    		break;
    	}
  	}while(pilihan != 0);

  	switch(pilihan){
    	case 1:
    		printf("Rekening Anda : %d\n", tempUser->rekening); //Untuk menampilkan nomor rekening pengguna
    		printf("Saldo Anda    : Rp%d\n", tempUser->saldo); //Untuk menampilkan saldo pengguna
    		pauseKey();
    		break;
        case 2:
            clearScreen();
            fhistory = fopen(tempUser->nama, "r");

            if(fhistory == NULL){
                /*
                Jika file nama nasabah tidak ada,
                maka artinya nasabah belum pernah melakukan transaksi.
                */
                printf("Info : %s belum pernah melakukan transaksi !\n", tempUser->nama);
				newFile(tempUser->nama);
            }
            else{
                if(head != NULL){
                    while(!feof(fhistory)){
                        fscanf(fhistory, "rekening=%d#nominal=%d#tipe=%d ", &rekening_tmp, &saldo_tmp, &tipe);

                        if(tipe == 1){ //tipe = 1 untuk Setor Tunai
							printf("Transaksi Ke-%d\n", i);
                            printf("Tipe Transaksi : Setor Tunai\n");
                            printf("Rekening Anda  : %d\n", tempUser->rekening);
                            printf("Jumlah Setoran : Rp%d\n", saldo_tmp);
                        }
                        else if(tipe == 2){ //tipe = 2 untuk Penarikan Tunai
							printf("Transaksi Ke-%d\n", i);
                            printf("Tipe Transaksi   : Penarikan Tunai\n");
                            printf("Rekening Anda    : %d\n", tempUser->rekening);
                            printf("Jumlah Penarikan : Rp%d\n", saldo_tmp);
                        }
                        else if(tipe == 3){ //tipe = 3 untuk Transfer
							printf("Transaksi Ke-%d\n", i);
                            printf("Tipe Transaksi  : Transfer\n");
                            printf("Rekening Anda   : %d\n", tempUser->rekening);
                            printf("Rekening Tujuan : %d\n", rekening_tmp);
                            printf("Jumlah Transfer : Rp%d\n", saldo_tmp);
                        }
                        printf("------------------------------------\n");
                        i++;
                    }
					if(tipe == 0){
						printf("Anda belum melakukan transaksi apapun!\n");
					} else {
						printf("Sukses menampilkan transaksi terakhir Anda\n");
					}
                }
                else {
                    printf("Belum Ada Nasabah yang Terdaftar !\n");
                }
            }
            fclose(fhistory);
            pauseKey();
            break;
    	default:
    		break;
  	}
}

void depositMenu(){
    int nominal = 0;

    clearScreen();
    printf("====================================\n");
    printf("          Menu Setor Tunai          \n");
    printf("====================================\n");
    printf("Masukkan Nominal Uang : Rp");
    nominal = askInt();

    tempUser->saldo += nominal; //Saldo pengguna bertambah sesuai nominal yang di input oleh pengguna

    replaceData(); //Untuk mereplace data baru pengguna di accounts.txt
    writeHistory(tempUser->rekening, nominal, 1); //Untuk menyimpan history hasil transaksi

    printf("\nSetor Tunai Berhasil !\n");
    pauseKey();
}

void withdrawMenu(){
	int pilihan = 0;
	int nominal[] = {100000, 500000, 1000000, 1500000, 2000000}; //Menyimpan nominal penarikan tunai ke dalam array

    do{
        clearScreen();
        printf("====================================\n");
        printf("        Menu Penarikan Tunai        \n");
    	printf("====================================\n");
    	printf("1. Rp100.000\n");
    	printf("2. Rp500.000\n");
    	printf("3. Rp1.000.000\n");
    	printf("4. Rp1.500.000\n");
    	printf("5. Rp2.000.000\n");
    	printf("0. Kembali\n");
    	printf("Pilihan : ");
        pilihan = askInt();

    	if(pilihan >= 0 && pilihan <= 5){
    		switch(pilihan){
        		case 1:
                    if(tempUser->saldo >= nominal[pilihan-1]){
            			tempUser->saldo -= nominal[pilihan-1]; //Saldo pengguna berkurang Rp100.000
            			writeHistory(tempUser->rekening, nominal[pilihan-1], 2); //Untuk menyimpan history hasil transaksi
            			printf("Penarikan Tunai Berhasil !\n");
                    }
                    else{
            			printf("Maaf Saldo Anda Tidak Mencukupi !\n");
                    }
          			break;
        		case 2:
          			if(tempUser->saldo >= nominal[pilihan-1]){
            			tempUser->saldo -= nominal[pilihan-1]; //Saldo pengguna berkurang Rp500.000
            			writeHistory(tempUser->rekening, nominal[pilihan-1], 2); //Untuk menyimpan history hasil transaksi
            			printf("Penarikan Tunai Berhasil !\n");
          			}
          			else{
            			printf("Maaf Saldo Anda Tidak Mencukupi !\n");
          			}
          			break;
        		case 3:
          			if(tempUser->saldo >= nominal[pilihan-1]){
            			tempUser->saldo -= nominal[pilihan-1]; //Saldo pengguna berkurang Rp1.000.000
            			writeHistory(tempUser->rekening, nominal[pilihan-1], 2); //Untuk menyimpan history hasil transaksi
            			printf("Penarikan Tunai Berhasil !\n");
          			}
          			else{
            			printf("Maaf Saldo Anda Tidak Mencukupi !\n");
          			}
          			break;
        		case 4:
          			if(tempUser->saldo >= nominal[pilihan-1]){
                        tempUser->saldo -= nominal[pilihan-1]; //Saldo pengguna berkurang Rp1.500.000
                        writeHistory(tempUser->rekening, nominal[pilihan-1], 2); //Untuk menyimpan history hasil transaksi
                        printf("Penarikan Tunai Berhasil !\n");
          			}
          			else{
            			printf("Maaf Saldo Anda Tidak Mencukupi !\n");
          			}
          			break;
        		case 5:
          			if(tempUser->saldo >= nominal[pilihan-1]){
            			tempUser->saldo -= nominal[pilihan-1]; //Saldo pengguna berkurang Rp2.000.000
            			writeHistory(tempUser->rekening, nominal[pilihan-1], 2); //Untuk menyimpan history hasil transaksi
            			printf("Penarikan Tunai Berhasil !\n");
          			}
          			else{
            			printf("Maaf Saldo Anda Tidak Mencukupi !\n");
          			}
          			break;
        		default:
          			break;
      		}

      		replaceData(); //Untuk mereplace data baru pengguna di accounts.txt

      		pauseKey();
            break;
    	}
		else{
    		printf("Pilihan Salah !");
    		pauseKey();
    	}
	}while(pilihan != 0);
}

void transferMenu(){
    int rekTujuan = 0, nominal = 0;

    clearScreen();
    printf("===================================\n");
    printf("           Menu Transfer           \n");
    printf("===================================\n");

    if(head != tail){
        printf("Masukkan Nomor Rekening Tujuan : ");
        rekTujuan = askInt();

        tempDirect = head;
        while(1){ //Untuk mencari rekening pengguna yang ingin di transfer
            if(rekTujuan == tempDirect->rekening && rekTujuan != tempUser->rekening)
                break;
            tempDirect = tempDirect->next;
            if(tempDirect == head)
                break;
        }

        if(rekTujuan == tempDirect->rekening && rekTujuan != tempUser->rekening){ //Jika rekening yang dituju valid, maka proses transfer dilakukan
            printf("Masukkan Nominal Saldo         : Rp");
            nominal = askInt();

            tempUser->saldo -= nominal;
            tempDirect->saldo += nominal;

            replaceData(); //Untuk mereplace data baru di accounts.txt
            writeHistory(rekTujuan, nominal, 3); //Untuk menyimpan history hasil transaksi

            printf("\nTransfer Berhasil !\n");
            pauseKey();
        }
        else { //Jika rekening pengguna tidak valid, maka tidak akan terjadi proses transfer
            printf("\nRekening Tujuan yang Anda Masukkan Tidak Terdaftar !\n");
            pauseKey();
        }
    }
    else { //Jika belum ada 2 rekening yang terdaftar di bank, maka tidak akan terjadi proses transfer
        printf("Minimal Harus Ada 2 Rekening yang Terdaftar untuk Melakukan Transfer Saldo !\n");
        pauseKey();
    }
}

void mainMenu(){
	int pilihan = 0;

	do{
		clearScreen();
		printf("====================================\n");
		printf("             Menu Utama             \n");
		printf("====================================\n");
		printf("1. Info\n");
		printf("2. Setor Tunai\n");
		printf("3. Tarik Tunai\n");
		printf("4. Transfer\n");
		printf("0. Kembali\n");
		printf("Pilihan : ");
		pilihan = askInt();

		switch(pilihan){
			case 1:
				infoMenu();
				break;
            case 2:
                depositMenu();
                break;
			case 3:
				withdrawMenu();
				break;
			case 4:
				transferMenu();
				break;
			default:
				break;
		}
	}while(pilihan != 0);
}

void swap(User *a, User *b){ //function untuk mengembalikan data yang sudah di sorting ke linked list
    char nama[MAX], pin[7];
    int kartu = 0, rekening = 0, saldo = 0, status = 0;

	strcpy(nama, a->nama);
	kartu = a->kartu;
	rekening = a->rekening;
	strcpy(pin, a->pin);
    saldo = a->saldo;
    status = a->status;

    strcpy(a->nama, b->nama);
    a->kartu = b->kartu;
    a->rekening = b->rekening;
    strcpy(a->pin, b->pin);
    a->saldo = b->saldo;
    a->status = b->status;

    strcpy(b->nama, nama);
    b->kartu = kartu;
    b->rekening = rekening;
    strcpy(b->pin, pin);
    b->saldo = saldo;
    b->status = status;
}

void sort_saldoTerkecil(){ //function untuk melakukan sorting dari saldo terkecil ke saldo terbesar menggunakan selection sort
    int i = 1;

    curr = head;
	if(head != NULL){
		while(curr->next != head){
		    temp_a = curr->next;
            temp_b = curr;
            while(1){
                if(temp_a->saldo < temp_b->saldo){
                    temp_b = temp_a;
                }
                temp_a = temp_a->next;
                if(temp_a == head){
                    break;
                }
            }

            if(curr != temp_b){
                swap(curr, temp_b);
            }
            curr = curr->next;
		}
	}
}

void sort_saldoTerbesar(){ //function untuk melakukan sorting dari saldo terbesar ke saldo terkecil menggunakan selection sort
    int i = 1;

    curr = head;
	if(head != NULL){
		while(curr->next != head){
		    temp_a = curr->next;
            temp_b = curr;
            while(1){
                if(temp_a->saldo > temp_b->saldo){
                    temp_b = temp_a;
                }
                temp_a = temp_a->next;
                if(temp_a == head){
                    break;
                }
            }

            if(curr != temp_b){
                swap(curr, temp_b);
            }
            curr = curr->next;
		}
	}
}

void daftarNasabah(char request[9]){ //function untuk menampilkan list nasabah yang sudah pernah mendaftar di bank
    int i = 1;
    clearScreen();
    printf("===================================\n");
    printf("         List Akun Nasabah         \n");
    printf("===================================\n");

    curr = head;
	if(head != NULL){
		while(1){
			printf("Data Nasabah Ke-%d\n", i);
			printf("Nama Nasabah   : %s\n", curr->nama);
			printf("Nomor Kartu    : %d\n", curr->kartu);
			printf("Nomor Rekening : %d\n", curr->rekening);
			printf("PIN            : %s\n", curr->pin);
			printf("Saldo          : Rp%d\n", curr->saldo);
			printf("Status         : %s\n", curr->status ? "Terblokir" : "Normal"); //Untuk melihat apakah data pengguna sudah terblokir atau belum
			printf("------------------------------------\n");
			curr = curr->next;
			i++;
			if(curr == head)
                break;
		}
        if(strcmp(request, "terbesar") == 0 && head != NULL){
            printf("\nDaftar Nasabah berhasil diurutkan berdasarkan saldo terbesar !\n");
        }
        else if(strcmp(request, "terkecil") == 0&& head != NULL){
            printf("\nDaftar Nasabah berhasil diurutkan berdasarkan saldo terkecil !\n");
        }
	}
	else{
		printf("Belum Ada Nasabah yang Terdaftar !\n");
	}

	pauseKey();
}

void listMenu_sorting(){
	int pilihan = 0;

	do{
        clearScreen();
        printf("===================================\n");
        printf("         List Data Nasabah         \n");
        printf("===================================\n");
        printf("1. Sort Saldo Terbesar\n");
        printf("2. Sort Saldo Terkecil\n");
        printf("0. Kembali\n");
        printf("Pilihan : ");
        pilihan = askInt();

        switch(pilihan){
            case 1:
                sort_saldoTerbesar();
                daftarNasabah("terbesar"); //Untuk melakukan sorting dan menampilkan dari saldo terbesar ke saldo terkecil
                break;
            case 2:
                sort_saldoTerkecil();
                daftarNasabah("terkecil"); //Untuk melakukan sorting dan menampilkan dari saldo terkecil ke saldo terbesar
                break;
            default:
                break;
        }
	}while(pilihan != 0);
}

void listMenu_tree(){
    Tree *root = NULL;
    int pilihan = 0;
    curr = head;

    root = insert(root, curr->kartu);

    curr = curr->next;
    while(1){
        insert(root, curr->kartu);
        curr = curr->next;
        if(curr == head)
            break;
    }

    do{
        clearScreen();
        printf("=====================================\n");
        printf("    List Data Nasabah (Mode Tree)    \n");
        printf("=====================================\n");
        printf("1. Pre-Order\n");
        printf("2. In-Order\n");
        printf("3. Post-Order\n");
        printf("0. Kembali\n");
        printf("Pilihan : ");
        pilihan = askInt();

		clearScreen();
        printf("=====================================\n");
        printf("    List Data Nasabah (Mode Tree)    \n");
        printf("=====================================\n");
        switch(pilihan){
            case 1:
                preorder(root);
                pauseKey();
                break;
            case 2:
                inorder(root);
                pauseKey();
                break;
            case 3:
                postorder(root);
                pauseKey();
                break;
            default:
                break;
        }
    }while(pilihan != 0);
}

void listMenu(){
    int pilihan = 0;

    do{
        clearScreen();
        printf("====================================\n");
        printf("       Menu List Data Nasabah       \n");
        printf("====================================\n");
        printf("1. Sorting Mode\n");
        printf("2. Tree Mode\n");
        printf("0. Kembali\n");
        printf("Pilihan : ");
        pilihan = askInt();

        switch(pilihan){
            case 1:
                listMenu_sorting();
                break;
            case 2:
                listMenu_tree();
                break;
            default:
                break;
        }
    }while(pilihan != 0);
}

void list_noKartu(){ //function untuk menampilkan urutan nomor kartu nasabah menggunakan algoritma hashing
    clearScreen();
    printf("====================================\n");
    printf("    Menu List Nomor Kartu Nasabah   \n");
    printf("====================================\n");

    for(int i = 0; i < HashT_MAX; i++){
        Hashing *ptr = HashT[i];

        printf("[%d] ->", i);
        while(ptr != NULL){
            printf(" (%d)", ptr->kartu);
            ptr = ptr->next;
        }
        printf("\n");
    }

    pauseKey();
}

void searchMenu(){ //function untuk mencari data nasabah dengan memasukkan nama nasabah
    char searchKey[MAX], temp[MAX];
    bool check = false;

    clearScreen();
    printf("====================================\n");
    printf("       Menu Pencarian Nasabah       \n");
    printf("====================================\n");
    printf("Masukkan nama lengkap nasabah : "); readLine(searchKey);
    printf("\n");

    curr = head;
    while(1){
        strcpy(temp, curr->nama);
        tolower_str(temp);
        tolower_str(searchKey);

        if(strcmp(searchKey, temp) == 0){
            check = true;
            break;
        }
        curr = curr->next;

        if(curr == head){
            break;
        }
    }

    if(check == true){
        printf("Data nasabah yang anda cari :\n");
        printf("Nama Nasabah   : %s\n", curr->nama);
        printf("Nomor Kartu    : %d\n", curr->kartu);
        printf("Nomor Rekening : %d\n", curr->rekening);
        printf("PIN            : %s\n", curr->pin);
        printf("Saldo          : Rp%d\n", curr->saldo);
        printf("Status         : %s\n", curr->status ? "Terblokir" : "Normal"); //Untuk melihat apakah data pengguna sudah terblokir atau belum
    } else {
        printf("Data pengguna tidak ditemukan !\n");
    }

    pauseKey();
}

void verificationMenu(){ //function untuk melakukan verifikasi akun menggunakan algoritma queue
    int pilihan;
    int tempPin;

    clearScreen();
    printf("====================================\n");
    printf("        Menu Verifikasi Akun        \n");
    printf("====================================\n");
    printf("Masukkan PIN (Administrator) : ");
    tempPin = askInt();

    if(tempPin == adminPin){
        if(!isEmpty(qhead)){
            while(!isEmpty(qhead)){
                do{
					clearScreen();
					printf("====================================\n");
        			printf("        Menu Verifikasi Akun        \n");
        			printf("====================================\n");
                    printf("Nama Nasabah   : %s\n", qhead->nama);
                    printf("Nomor Kartu    : %d\n", qhead->kartu);
                    printf("Nomor Rekening : %d\n", qhead->rekening);
                    printf("PIN            : %s\n", qhead->pin);
                    printf("Saldo          : Rp%d\n", qhead->saldo);
                    printf("Status         : %s\n", qhead->status ? "Terblokir" : "Normal"); //Untuk melihat apakah data pengguna sudah terblokir atau belum
                    printf("------------------------------------\n");
                    printf("1. Verifikasi akun\n");
                    printf("2. Batalkan akun\n");
                    printf("0. Kembali\n");
                    printf("Pilihan : ");
                    pilihan = askInt();

                    if(pilihan >= 0 && pilihan <= 2){
                        switch(pilihan){
                            case 1:
                                createNode(qhead->rekening, qhead->saldo, qhead->nama, qhead->pin, qhead->kartu, qhead->status); //Untuk membuat node baru di linked list
                                insertToChain(qhead->kartu, &HashT[qhead->kartu%10]);

                                fptr = fopen(filename, "a"); //Untuk mencatat data pengguna di accounts.txt
                                if(fptr == NULL){
                                    printf("\n Error: %s does not exist!", filename);
                                }
                                else{
                                    fprintf(fptr, "nama=%s#kartu=%d#rekening=%d#pin=%s#saldo=%d#blocked=%d\n", node->nama, node->kartu, node->rekening, node->pin, node->saldo, node->status);
                                }
                                fclose(fptr);

                                printf("Verifikasi akun berhasil!\n\n");
                                printf("Nomor Kartu Nasabah    : %d\n", node->kartu);
                                printf("Nomor Rekening Nasabah : %d\n", node->rekening);

                                dequeue();
                                pauseKey();
                                break;
                            case 2:
                                printf("Verifikasi akun dibatalkan!\n");

                                dequeue();
                                pauseKey();
                                break;
                            default:
                                break;
                        }

                        if(pilihan < 0 || pilihan > 2){
                            printf("Pilihan Salah !\n");
                            pauseKey();
                        }
                    }
                }while(pilihan < 0 || pilihan > 2);

                if(pilihan == 0) break;
            }
        }
        else{
            printf("Queue masih kosong!\n");
            pauseKey();
        }
    }
    else{
        printf("\nPIN salah!\n");
        pauseKey();
    }
}

void registerMenu(){ //function untuk melakukan pendaftaran
    int rekening = 0, saldo = 0, kartu = 0, status = 0;
	char nama[MAX], pin[MAX];

	kartu = randomNumber("card"); //Untuk generate nomor kartu acak
	rekening = randomNumber("rekening"); //Untuk generate nomor rekening acak

	clearScreen();
	printf("====================================\n");
	printf("      Menu Pendaftaran Nasabah      \n");
	printf("====================================\n");
	printf("Masukkan Nama Anda   : "); readLine(nama);

	printf("Masukkan Saldo Awal  : Rp");
	saldo = askInt();

	do{
        printf("Masukkan 6 Digit PIN : "); readLine(pin);
	}while(strlen(pin) != 6); //Untuk memastikan PIN yang dimasukkan pengguna harus 6 digit

	enqueue(rekening, saldo, nama, pin, kartu, status); //Untuk membuat node baru di linked list

    printf("-------------------------------------\n");
    printf("Akun Anda berhasil dimasukkan ke dalam Queue.\n");
    printf("Mohon tunggu hingga admin memverifikasi akun Anda !\n");
	pauseKey();
}

void loginMenu(){ //function untuk login
    int kartu = 0, tempKartu = 0, num_of_retries = 3, i = 0;
	char tempPin[MAX];
	bool retry = true, card_exist = false;
	Hashing *ptr;

	do{
        clearScreen();
	    printf("====================================\n");
        printf("             Menu Login             \n");
        printf("====================================\n");

        if(head != NULL){
            printf("Masukkan Nomor Kartu Anda : ");
            tempKartu = askInt();

            printf("Masukkan PIN Anda         : ");
            readLine(tempPin);


            /*
            Mencari input nomor kartu di HashTable,
            Jika input nomor kartu sesuai dengan yang ada di HashTable,
            maka akan keluar dari looping
            */
            for(i = 0; i < HashT_MAX; i++){
                ptr = HashT[i];

                if(ptr == NULL){
                    ptr = head_hash;
                } else {
                    while(ptr != NULL){
                        if(tempKartu == ptr->kartu){
                            card_exist = true;
                            break;
                        }
                        ptr = ptr->next;
                    }

                    if(card_exist){
                        kartu = ptr->kartu;
                        break;
                    }
                }
            }

            tempUser = head;
            while(1){ //Untuk mencari data pengguna yang login di linked list
                if(kartu == tempUser->kartu)
                    break;

                tempUser = tempUser->next;

                if(tempUser == head)
                    break;
            }


            if(kartu == tempUser->kartu && strcmp(tempPin, tempUser->pin) == 0 && tempUser->status == 0){ //Jika datanya valid maka masuk ke main menu
				newFile(tempUser->nama);
                mainMenu();
                break;
            }
			else if(tempUser->status == 1){ //Jika datanya sudah terblokir maka keluar dari menu login
                retry = false;
                break;
            }
			else if(kartu == tempUser->kartu && strcmp(tempPin, tempUser->pin) != 0 && tempUser->status == 0){ //Untuk melakukan hitung mundur, jika 3x berturut-turut pengguna salah memasukkan PIN maka datanya akan terblokir
                num_of_retries--;

                if(num_of_retries >= 1){
                    printf("\nPIN yang Anda masukkan salah !\n");
                    printf("Sisa percobaan Anda %d kali lagi\n", num_of_retries);

                    if(num_of_retries == 1){
                        printf("Jika Anda masih salah dalam memasukkan PIN Anda,\n");
                        printf("maka akun Anda akan TERBLOKIR !\n");
                    }

                    pauseKey();
                }

                if(num_of_retries == 0){
                    tempUser->status = 1;
                    replaceData();
                    retry = false;
                }
            }
			else{ //Jika data yang pengguna masukkan tidak valid atau tidak ada di linked list maka akan keluar dari menu login
                printf("\nData yang anda masukkan tidak terdaftar\n");
                pauseKey();
                break;
            }
        }
		else{ //Jika tidak ada satupun data di linked list atau pengguna belum pernah melakukan pendaftaran maka akan keluar dari menu login
            printf("Tidak Ada Nasabah yang Terdaftar !\n");
            pauseKey();
            break;
        }
    }while(retry == true);

    if(retry == false){
        printf("\nNomor Kartu Anda telah Terblokir!\n");
        pauseKey();
    }
}

void freeData(bool deleteData){
	User *temp;

    if(head != NULL){
        curr = head->next;
        while(curr != NULL && curr != head){
            temp = curr;
            curr = curr->next;
			if(deleteData){
				remove(temp->nama);
			}
            free(temp);
        }
		if(deleteData){
			remove(head->nama);
		}
        free(head);
        head = tail = NULL;
    }
}

void clearMenu(bool flag){
    if(flag){
        int tempPin;

        clearScreen();
        printf("=====================================\n");
        printf("    Menu Hapus Semua Data Nasabah    \n");
        printf("=====================================\n");
        printf("Masukkan PIN (Administrator) : ");
        tempPin = askInt();

        if(tempPin == adminPin){
            clearScreen();
            printf("=====================================\n");
            printf("    Menu Hapus Semua Data Nasabah    \n");
            printf("=====================================\n");
			freeData(flag);
            ftemp = fopen(tempFilename, "w");
            fclose(ftemp);
            remove(filename);
            rename(tempFilename, filename);

            printf("Semua Data Nasabah Berhasil Dihapus!\n");
        }
        else{
            printf("\nPIN Salah!\n");
        }
    }
    else{
        clearScreen();
		freeData(flag);
        printf("Terima Kasih Sudah Menggunakan U-Bank !\n");
    }

    pauseKey();
}

//function untuk mengecek keberadaan dan permission dari file accounts.txt
bool checkFile(){
	bool status = true;

	if(access(filename, F_OK) != 0){ // Check for file permission
		printf(" Error : '%s' does not exist!\n\n", filename);
		printf("A new file named '%s' has been created!\n", filename);
		FILE *file;
		file = fopen(filename, "a");
		fclose(file);
		pauseKey();
	} else {
		if(access(filename, R_OK) != 0){
			printf(" Error : '%s' file exist, but missing read permission!\n", filename);
			pauseKey();
			status =  false;
		}
		if(access(filename, W_OK) != 0){
			printf(" Error : '%s' file exist, but missing write permission!\n", filename);
			pauseKey();
			status =  false;
		}
	}
	return status;
}

int main(){
	int pilihan = 0;

	clearScreen();
	if(!checkFile()){
		return 0;
	}
    moveData(); //Untuk memindahkan data dari accounts.txt ke linked list sebelum program dimulai

	do{
		clearScreen();
		printf("====================================\n");
		printf("      Selamat Datang di U-Bank      \n");
		printf("====================================\n");
		printf("1. Login\n");
		printf("2. Register\n");
		printf("3. Verifikasi Akun Nasabah (Administrator)\n");
		printf("4. List Data Nasabah\n");
		printf("5. List Nomor Kartu Nasabah\n");
		printf("6. Cari Data Nasabah\n");
		printf("7. Hapus Semua Data Nasabah (Administrator)\n");
		printf("0. Keluar\n");
		printf("Pilihan : ");
		pilihan = askInt();

		if(pilihan >= 0 && pilihan <= 7){
			switch(pilihan){
				case 1:
					loginMenu();
					break;
				case 2:
					registerMenu();
					break;
				case 3:
                    verificationMenu(); // Untuk melakukan verifikasi akun menggunakan algoritma queue
					break;
				case 4:
                    listMenu(); // Untuk menampilkan data nasabah yang sudah pernah mendaftar
					break;
                case 5:
                    list_noKartu(); // Untuk menampilkan urutan nomor kartu nasabah menggunakan algoritma hashing
                    break;
                case 6:
                    searchMenu(); // Untuk mencari data nasabah dengan memasukkan nama nasabah
                    break;
                case 7:
                    clearMenu(true); // Untuk menghapus semua data di linked list dan di accounts.txt
                    break;
				default:
					break;
			}
		}
		else{
			printf("Pilihan Salah !");
			pauseKey();
		}
	}while(pilihan != 0);

	clearMenu(false); //Untuk menghapus semua data di linked list tanpa menghapus data di accounts.txt

	return 0;
}
