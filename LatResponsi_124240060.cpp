#include <iostream>
#include <string>
using namespace std;

const int TERSEDIA = 0;
const int DALAM_ANTREAN = 1;
const int SEDANG_DIPUTAR = 2;

struct Video {
    string judul;
    int menit;
    int detik;
    int status;
};

struct Node {
    Video data;
    Node* left;
    Node* right;
};

Node* root = NULL;
Node* playlist[100];  // Menyimpan pointer ke node
Video riwayat[100];
int jumlahPlaylist = 0, jumlahRiwayat = 0;

string aksiTerakhir = "";
Video videoUndo;
Node* nodeUndo = NULL;

void tampilkanStatus(int status) {
    if (status == TERSEDIA) cout << "Tersedia";
    else if (status == DALAM_ANTREAN) cout << "Dalam Antrean";
    else if (status == SEDANG_DIPUTAR) cout << "Sedang Diputar";
}

Node* tambah(Node* node, Video v) {
    if (node == NULL) {
        nodeUndo = new Node{v, NULL, NULL};
        aksiTerakhir = "tambah";
        return nodeUndo;
    }
    if (v.judul < node->data.judul) {
        node->left = tambah(node->left, v);
    } else if (v.judul > node->data.judul) {
        node->right = tambah(node->right, v);
    }
    return node;
}

Node* cari(Node* node, string judul) {
    if (node == NULL) return NULL;
    if (judul == node->data.judul) return node;
    else if (judul < node->data.judul) return cari(node->left, judul);
    else return cari(node->right, judul);
}

void tampilkanSemua(Node* node) {
    if (node == NULL) return;
    tampilkanSemua(node->left);
    cout << "- " << node->data.judul << " (" << node->data.menit << "m " << node->data.detik << "s) [";
    tampilkanStatus(node->data.status);
    cout << "]\n";
    tampilkanSemua(node->right);
}

void tampilkanVideoTersedia(Node* node) {
    if (node == NULL) return;
    tampilkanVideoTersedia(node->left);
    if (node->data.status == TERSEDIA) {
        cout << "- " << node->data.judul << " (" << node->data.menit << "m " << node->data.detik << "s)\n";
    }
    tampilkanVideoTersedia(node->right);
}

Node* hapus(Node* node, string judul) {
    if (node == NULL) return NULL;

    if (judul < node->data.judul) {
        node->left = hapus(node->left, judul);
    } else if (judul > node->data.judul) {
        node->right = hapus(node->right, judul);
    } else {
        videoUndo = node->data;
        aksiTerakhir = "hapus";

        for (int i = 0; i < jumlahPlaylist; i++) {
            if (playlist[i]->data.judul == judul) {
                for (int j = i; j < jumlahPlaylist - 1; j++)
                    playlist[j] = playlist[j + 1];
                jumlahPlaylist--;
                break;
            }
        }

        if (node->left == NULL && node->right == NULL) {
            delete node;
            return NULL;
        } else if (node->left == NULL) {
            Node* temp = node->right;
            delete node;
            return temp;
        } else if (node->right == NULL) {
            Node* temp = node->left;
            delete node;
            return temp;
        } else {
            Node* succ = node->right;
            while (succ->left != NULL) succ = succ->left;
            node->data = succ->data;
            node->right = hapus(node->right, succ->data.judul);
        }
    }
    return node;
}

void tambahVideo() {
    Video v;
    cin.ignore();
    cout << "\n------Tambah Video Baru------\n";
    cout << "Judul: "; getline(cin, v.judul);

    if (cari(root, v.judul)) {
        cout << "Judul video sudah ada! Gagal menambahkan.\n";
        return;
    }

    cout << "Durasi (menit): "; cin >> v.menit;
    cout << "Durasi (detik): "; cin >> v.detik;
    v.status = TERSEDIA;
    root = tambah(root, v);
    videoUndo = v;
    cout << "Video berhasil ditambahkan!\n";
}

void tampilkanDaftar() {
    cout << "\n------Daftar Video Tersimpan------\n";
    tampilkanSemua(root);
    char pilih;
    cout << "Ingin mencari video? (y/t): ";
    cin >> pilih;
    if (pilih == 'y') {
        cin.ignore();
        string j;
        cout << "Masukkan judul: "; getline(cin, j);
        Node* n = cari(root, j);
        if (n) {
            cout << "Ditemukan: " << n->data.judul << " [";
            tampilkanStatus(n->data.status);
            cout << "]\n";
        } else {
            cout << "Video tidak ditemukan.\n";
        }
    }
}

void tambahKePlaylist() {
    cout << "\n------Tambah ke Playlist------\n";
    cout << "Daftar Video Tersedia:\n";
    tampilkanVideoTersedia(root);

    cin.ignore();
    string j;
    cout << "Judul video: "; getline(cin, j);
    Node* n = cari(root, j);
    if (n == NULL) {
        cout << "Video tidak ditemukan.\n";
        return;
    }
    if (n->data.status != TERSEDIA) {
        cout << "Video tidak tersedia.\n";
        return;
    }

    n->data.status = (jumlahPlaylist == 0 ? SEDANG_DIPUTAR : DALAM_ANTREAN);
    playlist[jumlahPlaylist++] = n;
    videoUndo = n->data;
    aksiTerakhir = "playlist";
    cout << "Video berhasil ditambahkan ke playlist.\n";
}

void tampilkanPlaylist() {
    if (jumlahPlaylist == 0) {
        cout << "Playlist kosong.\n";
        return;
    }

    cout << "\n------ Playlist Saat Ini ------\n";
    for (int i = 0; i < jumlahPlaylist; i++) {
        cout << i + 1 << ". " << playlist[i]->data.judul << " [";
        tampilkanStatus(playlist[i]->data.status);
        cout << "]\n";
    }
}

void tontonVideo() {
    if (jumlahPlaylist == 0) {
        cout << "Playlist kosong.\n";
        return;
    }

    tampilkanPlaylist();

    Node* vidNode = playlist[0];
    cout << "\nSedang Memutar: " << vidNode->data.judul << "\n";

    for (int i = 1; i < jumlahPlaylist; i++) {
        playlist[i - 1] = playlist[i];
    }
    jumlahPlaylist--;

    vidNode->data.status = TERSEDIA;
    riwayat[jumlahRiwayat++] = vidNode->data;
    videoUndo = vidNode->data;
    aksiTerakhir = "tonton";

    if (jumlahPlaylist > 0)
        playlist[0]->data.status = SEDANG_DIPUTAR;
}

void tampilkanRiwayat() {
    cout << "\n------Riwayat Tontonan------\n";
    for (int i = jumlahRiwayat - 1; i >= 0; i--) {
        cout << riwayat[i].judul << "\n";
    }
}

void hapusVideo() {
    cin.ignore();
    string judul;
    cout << "\n------Hapus Video-------\n";
    cout << "Masukkan judul video: ";
    getline(cin, judul);

    Node* target = cari(root, judul);
    if (!target) {
        cout << "Video tidak ditemukan.\n";
        return;
    }

    if (target->data.status == SEDANG_DIPUTAR || target->data.status == DALAM_ANTREAN) {
        cout << "Video sedang " << (target->data.status == SEDANG_DIPUTAR ? "diputar" : "dalam antrean")
             << ". Yakin ingin menghapus? (y/t): ";
        char konfirmasi;
        cin >> konfirmasi;
        if (konfirmasi != 'y') {
            cout << "Penghapusan dibatalkan.\n";
            return;
        }
    }

    root = hapus(root, judul);
    cout << "Video berhasil dihapus.\n";
}

void undo() {
    if (aksiTerakhir == "tambah") {
        root = hapus(root, videoUndo.judul);
        cout << "Undo tambah video berhasil.\n";
    } else if (aksiTerakhir == "hapus") {
        root = tambah(root, videoUndo);
        cout << "Undo hapus video berhasil.\n";
    } else if (aksiTerakhir == "playlist") {
        jumlahPlaylist--;
        Node* n = cari(root, videoUndo.judul);
        if (n) n->data.status = TERSEDIA;
        cout << "Undo playlist berhasil.\n";
    } else if (aksiTerakhir == "tonton") {
        jumlahRiwayat--;
        Node* n = cari(root, videoUndo.judul);
        if (n) {
            playlist[jumlahPlaylist++] = n;
            n->data.status = SEDANG_DIPUTAR;
        }
        cout << "Undo tonton berhasil.\n";
    } else {
        cout << "Tidak ada aksi untuk di-undo.\n";
    }
    aksiTerakhir = "";
}

void menu() {
    int pilih;
    do {
        cout << "\n======================================\n";
        cout << "           MENU IDLIX TUBE            \n";
        cout << "======================================\n";
        cout << "1. Tambah Video\n";
        cout << "2. Tampilkan Video\n";
        cout << "3. Tambahkan ke Playlist\n";
        cout << "4. Tonton Video\n";
        cout << "5. Riwayat Tontonan\n";
        cout << "6. Hapus Video\n";
        cout << "7. Undo\n";
        cout << "0. Keluar\n";
        cout << "======================================\n";
        cout << "Pilih menu: ";
        cin >> pilih;

        switch (pilih) {
            case 1: tambahVideo(); break;
            case 2: tampilkanDaftar(); break;
            case 3: tambahKePlaylist(); break;
            case 4: tontonVideo(); break;
            case 5: tampilkanRiwayat(); break;
            case 6: hapusVideo(); break;
            case 7: undo(); break;
            case 0: cout << "Terima kasih telah menggunakan IDLIX Tube!\n"; break;
            default: cout << "Pilihan tidak valid.\n";
        }
    } while (pilih != 0);
}

int main() {
    menu();
    return 0;
}
