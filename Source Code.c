#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ODUNC 100


typedef struct Yazar {
    int id;
    char ad[50];
    char soyad[50];
    struct Yazar* next;
} Yazar;

Yazar* yazarlar = NULL;

typedef struct Ogrenci {
    int no;              // 8 haneli benzersiz numara
    char ad[50];
    char soyad[50];
    int puan;            // Baþlangýçta 100
    struct Ogrenci* prev;
    struct Ogrenci* next;
} Ogrenci;

Ogrenci* ogrenciler = NULL;

typedef struct KitapOdunc {
    int ogrenciNo;
    char etiketNo[20];
    int islemTuru; // 0: odunc alma, 1: teslim
    char tarih[11]; // GG/AA/YYYY
} KitapOdunc;

KitapOdunc oduncKayitlari[MAX_ODUNC];
int oduncSayisi = 0;

typedef struct Kitap {
    char ad[100];
    char isbn[14]; // 13 hane + '\0'
    int adet;
    struct Kitap* next;
} Kitap;

typedef struct KitapOrnek {
    char etiketNo[20];     // ISBN_1, ISBN_2 vs.
    char durum[20];        // RAFTA veya öðrenci no
    struct KitapOrnek* next;
} KitapOrnek;

Kitap* kitaplar = NULL;
KitapOrnek* kitapOrnekleri = NULL;




// Yeni yazar oluþturur
Yazar* yeniYazar(int id, const char* ad, const char* soyad) {
    Yazar* y = (Yazar*)malloc(sizeof(Yazar));
    if (y == NULL) return NULL;
    y->id = id;
    strcpy(y->ad, ad);
    strcpy(y->soyad, soyad);
    y->next = NULL;
    return y;
}

// Dosyadan yazarlarý okur
void yazarDosyadanOku() {
    FILE* fp = fopen("Yazarlar.csv", "r");
    if (fp == NULL) return;

    char satir[128];
    while (fgets(satir, sizeof(satir), fp) != NULL) {
        int id;
        char ad[50], soyad[50];
        if (sscanf(satir, "%d,%49[^,],%49[^\n]", &id, ad, soyad) == 3) {
            Yazar* y = yeniYazar(id, ad, soyad);
            if (yazarlar == NULL || id < yazarlar->id) {
                y->next = yazarlar;
                yazarlar = y;
            } else {
                Yazar* temp = yazarlar;
                while (temp->next != NULL && temp->next->id < id)
                    temp = temp->next;
                y->next = temp->next;
                temp->next = y;
            }
        }
    }

    fclose(fp);
}

// Dosyaya yazarlarý yazar
void yazarDosyayaKaydet() {
    FILE* fp = fopen("Yazarlar.csv", "w");
    if (fp == NULL) return;

    Yazar* temp = yazarlar;
    while (temp != NULL) {
        fprintf(fp, "%d,%s,%s\n", temp->id, temp->ad, temp->soyad);
        temp = temp->next;
    }

    fclose(fp);
}

// Yeni yazar ekle
void yazarEkle() {
    char ad[50], soyad[50];
    printf("Ad: ");
    scanf("%s", ad);
    printf("Soyad: ");
    scanf("%s", soyad);

    int yeniID = 1;
    Yazar* temp = yazarlar;
    while (temp != NULL) {
        if (temp->id >= yeniID)
            yeniID = temp->id + 1;
        temp = temp->next;
    }

    Yazar* yeni = yeniYazar(yeniID, ad, soyad);
    if (yeni == NULL) return;

    if (yazarlar == NULL || yeni->id < yazarlar->id) {
        yeni->next = yazarlar;
        yazarlar = yeni;
    } else {
        Yazar* curr = yazarlar;
        while (curr->next != NULL && curr->next->id < yeni->id)
            curr = curr->next;
        yeni->next = curr->next;
        curr->next = yeni;
    }

    yazarDosyayaKaydet();
    printf("Yazar eklendi.\n");
}

// Yazarlarý listele
void yazarListele() {
    Yazar* temp = yazarlar;
    while (temp != NULL) {
        printf("ID: %d - %s %s\n", temp->id, temp->ad, temp->soyad);
        temp = temp->next;
    }
}


void yazarSil() {
    int id;
    printf("Silinecek yazar ID: ");
    scanf("%d", &id);

    Yazar* temp = yazarlar;
    Yazar* onceki = NULL;

    while (temp != NULL) {
        if (temp->id == id) {
            if (onceki == NULL) {
                yazarlar = temp->next;
            } else {
                onceki->next = temp->next;
            }
            free(temp);
            yazarDosyayaKaydet();
            printf("Yazar silindi.\n");
            return;
        }
        onceki = temp;
        temp = temp->next;
    }

    printf("Yazar bulunamadi.\n");
}

void yazarGuncelle() {
    int id;
    char yeniAd[50], yeniSoyad[50];
    printf("Guncellenecek yazar ID: ");
    scanf("%d", &id);

    Yazar* temp = yazarlar;
    while (temp != NULL) {
        if (temp->id == id) {
            printf("Yeni ad: ");
            scanf("%s", yeniAd);
            printf("Yeni soyad: ");
            scanf("%s", yeniSoyad);
            strcpy(temp->ad, yeniAd);
            strcpy(temp->soyad, yeniSoyad);
            yazarDosyayaKaydet();
            printf("Yazar guncellendi.\n");
            return;
        }
        temp = temp->next;
    }

    printf("Yazar bulunamadi.\n");
}

void ogrenciDosyadanOku() {
    FILE* fp = fopen("Ogrenciler.csv", "r");
    if (fp == NULL) {
        return;
    }

    char satir[128];
    char ad[50], soyad[50];
    int no, puan;

    Ogrenci* son = NULL;

    while (fgets(satir, sizeof(satir), fp) != NULL) {
        if (sscanf(satir, "%d,%49[^,],%49[^,],%d", &no, ad, soyad, &puan) == 4) {
            Ogrenci* yeni = (Ogrenci*)malloc(sizeof(Ogrenci));
            if (yeni != NULL) {
                yeni->no = no;
                strcpy(yeni->ad, ad);
                strcpy(yeni->soyad, soyad);
                yeni->puan = puan;
                yeni->prev = NULL;
                yeni->next = NULL;

                if (ogrenciler == NULL) {
                    ogrenciler = yeni;
                } else {
                    son->next = yeni;
                    yeni->prev = son;
                }
                son = yeni;
            }
        }
    }

    fclose(fp);
}


void ogrenciDosyayaKaydet() {
    FILE* fp = fopen("Ogrenciler.csv", "w");
    if (fp == NULL) return;

    Ogrenci* temp = ogrenciler;
    while (temp != NULL) {
        fprintf(fp, "%d,%s,%s,%d\n", temp->no, temp->ad, temp->soyad, temp->puan);
        temp = temp->next;
    }

    fclose(fp);
}

void ogrenciEkle() {
    char ad[50], soyad[50];
    int no;

    printf("Ogrenci numarasi (8 hane): ");
    scanf("%d", &no);
    if (no < 10000000 || no > 99999999) {
        printf("Gecersiz numara.\n");
        return;
    }

    // Benzersiz mi kontrol et
    Ogrenci* temp = ogrenciler;
    while (temp != NULL) {
        if (temp->no == no) {
            printf("Bu numara zaten var.\n");
            return;
        }
        temp = temp->next;
    }

    printf("Ad: ");
    scanf("%s", ad);
    printf("Soyad: ");
    scanf("%s", soyad);

    Ogrenci* yeni = (Ogrenci*)malloc(sizeof(Ogrenci));
    if (yeni == NULL) return;

    yeni->no = no;
    strcpy(yeni->ad, ad);
    strcpy(yeni->soyad, soyad);
    yeni->puan = 100;
    yeni->next = NULL;
    yeni->prev = NULL;

    if (ogrenciler == NULL) {
        ogrenciler = yeni;
    } else {
        Ogrenci* son = ogrenciler;
        while (son->next != NULL) son = son->next;
        son->next = yeni;
        yeni->prev = son;
    }

    ogrenciDosyayaKaydet();
    printf("Ogrenci eklendi.\n");
}

void ogrenciListele() {
    Ogrenci* temp = ogrenciler;
    while (temp != NULL) {
        printf("No: %d - %s %s - Puan: %d\n", temp->no, temp->ad, temp->soyad, temp->puan);
        temp = temp->next;
    }
}

void ogrenciSil() {
    int no;
    printf("Silinecek ogrenci numarasi: ");
    scanf("%d", &no);

    Ogrenci* temp = ogrenciler;

    while (temp != NULL) {
        if (temp->no == no) {
            if (temp->prev != NULL)
                temp->prev->next = temp->next;
            else
                ogrenciler = temp->next;

            if (temp->next != NULL)
                temp->next->prev = temp->prev;

            free(temp);
            ogrenciDosyayaKaydet();
            printf("Ogrenci silindi.\n");
            return;
        }
        temp = temp->next;
    }

    printf("Ogrenci bulunamadi.\n");
}


void ogrenciGuncelle() {
    int no;
    printf("Guncellenecek ogrenci numarasi: ");
    scanf("%d", &no);

    Ogrenci* temp = ogrenciler;

    while (temp != NULL) {
        if (temp->no == no) {
            char yeniAd[50], yeniSoyad[50];
            printf("Yeni ad: ");
            scanf("%s", yeniAd);
            printf("Yeni soyad: ");
            scanf("%s", yeniSoyad);

            strcpy(temp->ad, yeniAd);
            strcpy(temp->soyad, yeniSoyad);
            ogrenciDosyayaKaydet();
            printf("Ogrenci guncellendi.\n");
            return;
        }
        temp = temp->next;
    }

    printf("Ogrenci bulunamadi.\n");
}

void oduncDosyasiniYukle() {
    FILE* fp = fopen("KitapOdunc.csv", "r");
    if (fp == NULL) return;

    char satir[128];
    while (fgets(satir, sizeof(satir), fp) != NULL && oduncSayisi < MAX_ODUNC) {
        KitapOdunc k;
        if (sscanf(satir, "%d,%[^,],%d,%[^,\n]", &k.ogrenciNo, k.etiketNo, &k.islemTuru, k.tarih) == 4) {
            oduncKayitlari[oduncSayisi++] = k;
        }
    }

    fclose(fp);
}

void ogrenciBilgisiGoster() {
    int no;
    printf("Ogrenci numarasi: ");
    scanf("%d", &no);

    Ogrenci* temp = ogrenciler;
    while (temp != NULL) {
        if (temp->no == no) {
            printf("Ad: %s\nSoyad: %s\nNo: %d\nPuan: %d\n", temp->ad, temp->soyad, temp->no, temp->puan);
            
            oduncDosyasiniYukle();
            printf("Kitap Hareketleri:\n");
            int i = 0;
            while (i < oduncSayisi) {
                if (oduncKayitlari[i].ogrenciNo == no) {
                    printf("Etiket: %s - Islem: %s - Tarih: %s\n",
                        oduncKayitlari[i].etiketNo,
                        oduncKayitlari[i].islemTuru == 0 ? "Odunc Alindi" : "Teslim Edildi",
                        oduncKayitlari[i].tarih
                    );
                }
                i++;
            }
            return;
        }
        temp = temp->next;
    }

    printf("Ogrenci bulunamadi.\n");
}

void teslimEtmemisleriListele() {
    oduncDosyasiniYukle();
    int i = 0;

    while (i < oduncSayisi) {
        if (oduncKayitlari[i].islemTuru == 0) {
            int teslimEdildi = 0;
            int j = i + 1;

            while (j < oduncSayisi && teslimEdildi == 0) {
                if (strcmp(oduncKayitlari[i].etiketNo, oduncKayitlari[j].etiketNo) == 0 &&
                    oduncKayitlari[j].islemTuru == 1 &&
                    oduncKayitlari[j].ogrenciNo == oduncKayitlari[i].ogrenciNo) {
                    teslimEdildi = 1;
                }
                j++;
            }

            if (teslimEdildi == 0) {
                printf("Ogrenci No: %d - Etiket: %s\n",
                    oduncKayitlari[i].ogrenciNo, oduncKayitlari[i].etiketNo);
            }
        }
        i++;
    }
}


void cezaliOgrencileriListele() {
    Ogrenci* temp = ogrenciler;
    while (temp != NULL) {
        if (temp->puan <= 0) {
            printf("No: %d - %s %s - Puan: %d\n", temp->no, temp->ad, temp->soyad, temp->puan);
        }
        temp = temp->next;
    }
}

void kitaplariYukle() {
    FILE* fp = fopen("Kitaplar.csv", "r");
    if (fp == NULL) {
        return;
    }

    char satir[128];
    char ad[100], isbn[14];
    int adet;
    Kitap* son = NULL;

    while (fgets(satir, sizeof(satir), fp) != NULL) {
        if (sscanf(satir, "%99[^,],%13[^,],%d", ad, isbn, &adet) == 3) {
            Kitap* yeni = (Kitap*)malloc(sizeof(Kitap));
            if (yeni != NULL) {
                strcpy(yeni->ad, ad);
                strcpy(yeni->isbn, isbn);
                yeni->adet = adet;
                yeni->next = NULL;

                if (kitaplar == NULL) {
                    kitaplar = yeni;
                } else {
                    son->next = yeni;
                }
                son = yeni;
            }
        }
    }

    fclose(fp);
}


void kitaplariKaydet() {
    FILE* fp = fopen("Kitaplar.csv", "w");
    if (fp == NULL) return;

    Kitap* temp = kitaplar;
    while (temp != NULL) {
        fprintf(fp, "%s,%s,%d\n", temp->ad, temp->isbn, temp->adet);
        temp = temp->next;
    }

    fclose(fp);
}

void kitapEkle() {
    char ad[100], isbn[14];
    int adet;

    printf("Kitap adi: ");
    scanf(" %[^\n]", ad); // Boþluklu okuma
    printf("ISBN (13 hane): ");
    scanf("%s", isbn);
    printf("Adet sayisi: ");
    scanf("%d", &adet);

    // Ayný ISBN varsa engelle
    Kitap* tmp = kitaplar;
    while (tmp != NULL) {
        if (strcmp(tmp->isbn, isbn) == 0) {
            printf("Bu ISBN zaten var.\n");
            return;
        }
        tmp = tmp->next;
    }

    Kitap* yeni = (Kitap*)malloc(sizeof(Kitap));
    if (yeni == NULL) return;

    strcpy(yeni->ad, ad);
    strcpy(yeni->isbn, isbn);
    yeni->adet = adet;
    yeni->next = NULL;

    if (kitaplar == NULL) {
        kitaplar = yeni;
    } else {
        Kitap* temp = kitaplar;
        while (temp->next != NULL) temp = temp->next;
        temp->next = yeni;
    }

    // Kitap örneklerini dosyaya yaz
    FILE* fp = fopen("KitapOrnekleri.csv", "a");
    if (fp == NULL) return;

    int i = 1;
    while (i <= adet) {
        fprintf(fp, "%s_%d,RAFTA\n", isbn, i);
        i++;
    }

    kitaplariKaydet();
    printf("Kitap ve ornekleri eklendi.\n");
}

void kitapListele() {
    Kitap* temp = kitaplar;
    while (temp != NULL) {
        printf("Ad: %s | ISBN: %s | Adet: %d\n", temp->ad, temp->isbn, temp->adet);
        temp = temp->next;
    }
}

void kitapSil() {
    char isbn[14];
    printf("Silinecek kitap ISBN: ");
    scanf("%s", isbn);

    Kitap* temp = kitaplar;
    Kitap* onceki = NULL;

    while (temp != NULL) {
        if (strcmp(temp->isbn, isbn) == 0) {
            if (onceki == NULL)
                kitaplar = temp->next;
            else
                onceki->next = temp->next;

            free(temp);
            kitaplariKaydet();

            // KitapOrnekleri.csv'den sil
            FILE* eski = fopen("KitapOrnekleri.csv", "r");
            FILE* yeni = fopen("tmp.csv", "w");
            char satir[100];
            while (fgets(satir, sizeof(satir), eski)) {
                if (strncmp(satir, isbn, strlen(isbn)) != 0) {
                    fputs(satir, yeni);
                }
            }
            fclose(eski);
            fclose(yeni);
            remove("KitapOrnekleri.csv");
            rename("tmp.csv", "KitapOrnekleri.csv");

            printf("Kitap ve ornekleri silindi.\n");
            return;
        }
        onceki = temp;
        temp = temp->next;
    }

    printf("Kitap bulunamadi.\n");
}

void kitapGuncelle() {
    char isbn[14];
    printf("Guncellenecek kitap ISBN: ");
    scanf("%s", isbn);

    Kitap* temp = kitaplar;
    while (temp != NULL) {
        if (strcmp(temp->isbn, isbn) == 0) {
            printf("Yeni ad: ");
            scanf(" %[^\n]", temp->ad);

            int yeniAdet;
            printf("Yeni adet: ");
            scanf("%d", &yeniAdet);

            if (yeniAdet > temp->adet) {
                FILE* fp = fopen("KitapOrnekleri.csv", "a");
                int i = temp->adet + 1;
                while (i <= yeniAdet) {
                    fprintf(fp, "%s_%d,RAFTA\n", isbn, i);
                    i++;
                }
                fclose(fp);
            }
            temp->adet = yeniAdet;
            kitaplariKaydet();
            printf("Kitap guncellendi.\n");
            return;
        }
        temp = temp->next;
    }

    printf("Kitap bulunamadi.\n");
}

void kitapBilgisiGoster() {
    char kitapAdi[100];
    printf("Aranacak kitap adi: ");
    scanf(" %[^\n]", kitapAdi);

    Kitap* temp = kitaplar;
    while (temp != NULL) {
        if (strcmp(temp->ad, kitapAdi) == 0) {
            printf("Ad: %s\nISBN: %s\nAdet: %d\n", temp->ad, temp->isbn, temp->adet);

            FILE* fp = fopen("KitapOrnekleri.csv", "r");
            if (fp == NULL) {
                printf("Ornek dosyasi bulunamadi.\n");
                return;
            }

            char satir[100];
            printf("Ornekler:\n");
            while (fgets(satir, sizeof(satir), fp)) {
                char etiket[20], durum[20];
                if (sscanf(satir, "%[^,],%s", etiket, durum) == 2) {
                    if (strncmp(etiket, temp->isbn, strlen(temp->isbn)) == 0) {
                        printf("Etiket: %s - Durum: %s\n", etiket, durum);
                    }
                }
            }

            fclose(fp);
            return;
        }
        temp = temp->next;
    }

    printf("Kitap bulunamadi.\n");
}

void raftakiKitaplariListele() {
    FILE* fp = fopen("KitapOrnekleri.csv", "r");
    if (fp == NULL) {
        printf("KitapOrnekleri.csv dosyasý bulunamadý.\n");
        return;
    }

    char satir[100];
    int bulundu = 0;

    printf("RAFTA OLAN KÝTAPLAR:\n");
    while (fgets(satir, sizeof(satir), fp)) {
        char etiketNo[20], durum[20];
        if (sscanf(satir, "%[^,],%s", etiketNo, durum) == 2) {
            if (strcmp(durum, "RAFTA") == 0) {
                printf("Etiket: %s\n", etiketNo);
                bulundu = 1;
            }
        }
    }

    if (!bulundu) {
        printf("Rafta kitap bulunmamaktadýr.\n");
    }

    fclose(fp);
}

int tarihToGun(const char* tarih) {
    int gun = 0, ay = 0, yil = 0;
    sscanf(tarih, "%d/%d/%d", &gun, &ay, &yil);
    return yil * 365 + ay * 30 + gun; // Basit hesaplama, yýl-ay-gün farký
}

void zamanindaTeslimEdilmeyenleriListele() {
    int i = 0;
    int j = 0;
    int teslimGec = 0;

    oduncDosyasiniYukle();

    printf("Zamanýnda Teslim Edilmeyen Kitaplar:\n");

    i = 0;
    while (i < oduncSayisi) {
        if (oduncKayitlari[i].islemTuru == 0) { // Ödünç alýnmýþ
            j = i + 1;
            while (j < oduncSayisi) {
                if (oduncKayitlari[j].islemTuru == 1 &&
                    strcmp(oduncKayitlari[i].etiketNo, oduncKayitlari[j].etiketNo) == 0 &&
                    oduncKayitlari[i].ogrenciNo == oduncKayitlari[j].ogrenciNo) {

                    int gunFarki = tarihToGun(oduncKayitlari[j].tarih) - tarihToGun(oduncKayitlari[i].tarih);
                    if (gunFarki > 15) {
                        printf("Ogrenci No: %d | Etiket: %s | Gec Teslim: %d gün\n",
                               oduncKayitlari[i].ogrenciNo,
                               oduncKayitlari[i].etiketNo,
                               gunFarki);
                        teslimGec = 1;
                    }
                    j = oduncSayisi; // Döngüden çýkmak için üst sýnýrý zorla
                }
                j++;
            }
        }
        i++;
    }

    if (!teslimGec) {
        printf("Tüm kitaplar zamanýnda teslim edilmis.\n");
    }
}

int kitapVarMi(const char* isbn) {
    Kitap* temp = kitaplar;
    while (temp != NULL) {
        if (strcmp(temp->isbn, isbn) == 0) {
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

int yazarVarMi(int yazarID) {
    Yazar* temp = yazarlar;
    while (temp != NULL) {
        if (temp->id == yazarID) {
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

int eslesmeVarMi(const char* isbn, int yazarID) {
    FILE* fp = fopen("KitapYazar.csv", "r");
    if (fp == NULL) return 0; // Dosya yoksa eþleþme yoktur

    char satir[128];
    int id;
    char satirISBN[20];

    while (fgets(satir, sizeof(satir), fp)) {
        if (sscanf(satir, "%[^,],%d", satirISBN, &id) == 2) {
            if (strcmp(satirISBN, isbn) == 0 && id == yazarID) {
                fclose(fp);
                return 1;
            }
        }
    }

    fclose(fp);
    return 0;
}

void kitapYazarEsle() {
    char isbn[14];
    int yazarID;
    int devam = 1;

    printf("Eslestirme yapýlacak kitap ISBN: ");
    scanf("%s", isbn);

    if (!kitapVarMi(isbn)) {
        printf("Kitap bulunamadi.\n");
        return;
    }

    FILE* fp = fopen("KitapYazar.csv", "a");
    if (fp == NULL) {
        printf("Dosya acilamadi.\n");
        return;
    }

    while (devam) {
        printf("Yazar ID (0 girerseniz cikis yapilir): ");
        scanf("%d", &yazarID);

        if (yazarID == 0) {
            devam = 0;
        } else if (!yazarVarMi(yazarID)) {
            printf("Yazar bulunamadi.\n");
        } else if (eslesmeVarMi(isbn, yazarID)) {
            printf("Bu yazar zaten bu kitapla eslestirilmis.\n");
        } else {
            fprintf(fp, "%s,%d\n", isbn, yazarID);
            printf("Esleme kaydedildi.\n");
        }
    }

    fclose(fp);
}

void kitabinYazariniGuncelle() {
    char isbn[14];
    int yeniYazarID;
    int bulundu = 0;

    printf("Guncellenecek kitap ISBN: ");
    scanf("%s", isbn);

    if (!kitapVarMi(isbn)) {
        printf("Kitap bulunamadi.\n");
        return;
    }

    printf("Yeni yazar ID: ");
    scanf("%d", &yeniYazarID);

    if (!yazarVarMi(yeniYazarID)) {
        printf("Yazar bulunamadi.\n");
        return;
    }

    FILE* eski = fopen("KitapYazar.csv", "r");
    FILE* yeni = fopen("tmp.csv", "w");
    char satir[128], satirISBN[20];
    int yazarID;

    while (fgets(satir, sizeof(satir), eski)) {
        if (sscanf(satir, "%[^,],%d", satirISBN, &yazarID) == 2) {
            if (strcmp(satirISBN, isbn) == 0) {
                fprintf(yeni, "%s,%d\n", isbn, yeniYazarID);
                bulundu = 1;
            } else {
                fprintf(yeni, "%s,%d\n", satirISBN, yazarID);
            }
        }
    }

    fclose(eski);
    fclose(yeni);
    remove("KitapYazar.csv");
    rename("tmp.csv", "KitapYazar.csv");

    if (bulundu)
        printf("Yazar guncellendi.\n");
    else
        printf("Bu kitaba ait yazar eslesmesi bulunamadi. Yeni olarak eklendi.\n");
}

void kitapOduncIslemi() {
    int no, islemTuru;
    char isbn[14];
    char tarih[11];
    int bulundu = 0;

    printf("Ogrenci numarasi: ");
    scanf("%d", &no);

    Ogrenci* ogr = ogrenciler;
    while (ogr != NULL && bulundu == 0) {
        if (ogr->no == no) {
            bulundu = 1;
        } else {
            ogr = ogr->next;
        }
    }

    if (bulundu == 0) {
        printf("Ogrenci bulunamadi.\n");
        return;
    }

    if (ogr->puan <= 0) {
        printf("Puan yetersiz. Islemi yapamaz.\n");
        return;
    }

    printf("ISBN: ");
    scanf("%s", isbn);

    printf("Tarih (GG/AA/YYYY): ");
    scanf("%s", tarih);

    printf("Islem turu (0=Odunc, 1=Teslim): ");
    scanf("%d", &islemTuru);

    FILE* fp = fopen("KitapOrnekleri.csv", "r");
    FILE* fpYeni = fopen("tmp.csv", "w");

    char satir[100], etiketNo[20], durum[20];
    int islemYapildi = 0;

    while (fgets(satir, sizeof(satir), fp)) {
        sscanf(satir, "%[^,],%s", etiketNo, durum);

        int etiketEslesiyor = (strncmp(etiketNo, isbn, strlen(isbn)) == 0);

        if (islemYapildi == 0 && etiketEslesiyor) {
            if (islemTuru == 0 && strcmp(durum, "RAFTA") == 0) {
                fprintf(fpYeni, "%s,%d\n", etiketNo, no);
                islemYapildi = 1;
            } else if (islemTuru == 1 && atoi(durum) == no) {
                fprintf(fpYeni, "%s,RAFTA\n", etiketNo);
                islemYapildi = 1;

                // Gec teslim kontrolü
                int i = 0;
                int teslimKontrolYapildi = 0;

                while (i < oduncSayisi && teslimKontrolYapildi == 0) {
                    int ayniOgrenci = (oduncKayitlari[i].ogrenciNo == no);
                    int ayniEtiket = (strcmp(oduncKayitlari[i].etiketNo, etiketNo) == 0);
                    int oduncMu = (oduncKayitlari[i].islemTuru == 0);

                    if (ayniOgrenci && ayniEtiket && oduncMu) {
                        int gunFark = tarihToGun(tarih) - tarihToGun(oduncKayitlari[i].tarih);
                        if (gunFark > 15) {
                            ogr->puan -= 10;
                            printf("Gec teslim (-10 puan)\n");
                        }
                        teslimKontrolYapildi = 1;
                    }
                    i++;
                }

                ogrenciDosyayaKaydet();
            } else {
                fprintf(fpYeni, "%s,%s\n", etiketNo, durum);
            }
        } else {
            fprintf(fpYeni, "%s,%s\n", etiketNo, durum);
        }
    }

    fclose(fp);
    fclose(fpYeni);
    remove("KitapOrnekleri.csv");
    rename("tmp.csv", "KitapOrnekleri.csv");

    if (islemYapildi) {
        FILE* odunc = fopen("KitapOdunc.csv", "a");
        fprintf(odunc, "%d,%s,%d,%s\n", no, etiketNo, islemTuru, tarih);
        fclose(odunc);
        printf("Islem kaydedildi.\n");
    } else {
        printf("Islem basarisiz.\n");
    }
}


void yazarBilgisiGoster() {
    char arananAd[50];
    printf("Yazar adi: ");
    scanf("%s", arananAd);

    Yazar* y = yazarlar;
    int bulundu = 0;

    while (y != NULL) {
        if (strcmp(y->ad, arananAd) == 0) {
            printf("Yazar ID: %d | %s %s\n", y->id, y->ad, y->soyad);
            bulundu = 1;

            FILE* fp = fopen("KitapYazar.csv", "r");
            char satir[128], isbn[20];
            int yid;
            printf("Yazarin kitaplari:\n");
            while (fgets(satir, sizeof(satir), fp)) {
                sscanf(satir, "%[^,],%d", isbn, &yid);
                if (yid == y->id) {
                    Kitap* k = kitaplar;
                    while (k != NULL) {
                        if (strcmp(k->isbn, isbn) == 0) {
                            printf("- %s (ISBN: %s)\n", k->ad, k->isbn);
                        }
                        k = k->next;
                    }
                }
            }
            fclose(fp);
        }
        y = y->next;
    }

    if (!bulundu)
        printf("Yazar bulunamadi.\n");
}

int main() {
    int secim;

    yazarDosyadanOku();
    ogrenciDosyadanOku();
    kitaplariYukle();
    oduncDosyasiniYukle();

    do {
        printf("\n--- KÜTÜPHANE OTOMASYONU ---\n");
        printf("1 - Ogrenci Ekle\n");
        printf("2 - Ogrenci Sil\n");
        printf("3 - Ogrenci Guncelle\n");
        printf("4 - Ogrenci Bilgisi\n");
        printf("5 - Teslim Etmemisleri Listele\n");
        printf("6 - Cezali Ogrencileri Listele\n");
        printf("7 - Kitap Ekle\n");
        printf("8 - Kitap Sil\n");
        printf("9 - Kitap Guncelle\n");
        printf("10 - Kitap Bilgisi Goruntule\n");
        printf("11 - Raftaki Kitaplari Listele\n");
        printf("12 - Zamaninda Teslim Edilmeyenleri Listele\n");
        printf("13 - Kitap-Yazar Eslestir\n");
        printf("14 - Kitabin Yazarini Guncelle\n");
        printf("15 - Yazar Ekle\n");
        printf("16 - Yazar Sil\n");
        printf("17 - Yazar Guncelle\n");
        printf("18 - Yazar Bilgisi Goruntule\n");
        printf("19 - Kitap Odunc Al/Teslim Et\n");
        printf("0 - CIKIS\n");

        printf("Secim: ");
        scanf("%d", &secim);

        if (secim == 1) ogrenciEkle();
        else if (secim == 2) ogrenciSil();
        else if (secim == 3) ogrenciGuncelle();
        else if (secim == 4) ogrenciBilgisiGoster();
        else if (secim == 5) teslimEtmemisleriListele();
        else if (secim == 6) cezaliOgrencileriListele();
        else if (secim == 7) kitapEkle();
        else if (secim == 8) kitapSil();
        else if (secim == 9) kitapGuncelle();
        else if (secim == 10) kitapBilgisiGoster();
        else if (secim == 11) raftakiKitaplariListele();
        else if (secim == 12) zamanindaTeslimEdilmeyenleriListele();
        else if (secim == 13) kitapYazarEsle();
        else if (secim == 14) kitabinYazariniGuncelle();
        else if (secim == 15) yazarEkle();
        else if (secim == 16) yazarSil();
        else if (secim == 17) yazarGuncelle();
        else if (secim == 18) yazarBilgisiGoster();
        else if (secim == 19) kitapOduncIslemi();
        else if (secim != 0) printf("Gecersiz secim.\n");

    } while (secim != 0);

    printf("Program sonlandi.\n");
    return 0;
}











