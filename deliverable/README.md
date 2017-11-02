# Tugas Jaringan Komunikasi : Sliding Window Kelompok Cloud

## Anggota Tim

1. Trevin Matthew Robertsen (13515027)
2. Edwin Kumara Tandiono (13515039)
3. Albertus Djauhari Djohan (13515054)

## Petunjuk cara menggunakan program

Gunakan makefile untuk kompilasi sender dan receiver. Gunakan command berikut untuk menjalankan program:
./sendfile <filename> <windowsize> <buffersize> <destination-ip> <destination_port>
./recvfile​ ​ <filename>​ ​ <windowsize>​ ​ <buffersize>​ ​ <port>

## Cara kerja sliding window

Ada dua program untuk mensimulasikan sliding window. Program pertama adalah sendfile.c, program kedua adalah recvfile.c. sendfile.c akan membaca data dari file eksternal seukuran buffersize dan disimpan di buff. Kemudian, program akan mengirim data yang sudah dimasukan ke segment sebanyak ukuran window size. Kemudian, program akan menunggu ack sampai timeout. Di sisi program recvfile.c, program akan menerima paket data yang dikirimkan. Jika ada paket data yang sampai, program akan melakukan validasi terhadap paket yang diterima. Jika valid, maka data akan dimasukan ke buff dan ack akan dikirim ke program sendfile.c. Data akan dimasukkan terus ke buff sampai buff penuh. Jika buff telah penuh, maka data akan ditulis ke file eksternal dan buff akan dikosongkan. Jika pada sendfile.c ack diterima sebelum timeout, maka akan ditandai pada array bahwa ack tersebut telah diterima. Jika file yang dibaca oleh senderfile.c telah habis, maka akan dikirim paket dengan sequence number -1.

## Pembagian tugas

1. Trevin Matthew Robertsen : Read/write file
2. Edwin Kumara Tandiono : socket connection, segment
3. Albertus Djauhari Djohan : sliding window, send dan receive

## Jawaban pertanyaan

Soal 1. 
Jika windowsize bernilai 0, baik pengirim maupun penerima paket tidak akan dapat memproses data yang dikirim/diterima.
Untuk menangani hal ini, kita dapat menggunakan timeout sehingga sistem akan berhenti untuk berusaha mengirimkan paket pada keadaan window size = 0 setelah beberapa lama waktu berjalan.
 
Soal 2.
TCP header memiliki field sbb:
1. Source TCP port number (2 byte) adalah ujung pengirim dari komunikasi jaringan
2. Destination TCP port number (2 byte) adalah ujung penerima dari komunikasi jaringan
3. Sequence number (4 byte) digunakan untuk menandai urutan dari pengiriman data
4. Acknowledgement number (4 byte) digunakan untuk menandai data apa saja beserta urutannya yang telah diterima
5. TCP data offset (4 bit) digunakan untuk menyimpan ukuran total header TCP
6. Reserved data (3 bit) digunakan sebagai 'komplemen' dari header sehingga kerja lebih efisien
7. Control flags (<=9 bit) digunakan untuk manajemen aliran data dalam situasi - situasi tertentu
8. Window size (2 bit) digunakan untuk regulasi banyaknya data yang dapat dikirim ke receiver sebelum harus menunggu ACK-nya
9. TCP checksum (2 byte) digunakan untuk pengecekan error
10. Urgent pointer (2 byte) dapat digunakan layaknya control flag untuk menandai prioritas
11. TCP optional data (0 ~ 40 byte) dapat digunakan untuk menyimpan data - data opsional