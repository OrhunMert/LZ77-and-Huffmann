#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h> // NULL
#include <stdlib.h> // calloc
#include <time.h> // süre bulmak için

#define OFFSETBITS 5
#define LENGTHBITS (8-OFFSETBITS)

#define OFFSETMASK ((1 << (OFFSETBITS)) - 1)
#define LENGTHMASK ((1 << (LENGTHBITS)) - 1)

#define GETOFFSET(x) (x >> LENGTHBITS)
#define GETLENGTH(x) (x & LENGTHMASK)
#define OFFSETLENGTH(x,y) (x << LENGTHBITS | y)

typedef unsigned char byte;

struct token
{
    uint8_t offset_len;
    char c;
};


typedef struct node_t
{
    struct node_t *left, *right;
    int freq;
    char c;
} *node;
struct EnCode
{
    char BitKodu;
};

struct node_t pool[256] = {{0}};
node qqq[255], *q = qqq - 1;
int n_nodes = 0, qend = 1;
char *code[10024] = {0}, buf[10024];

node new_node(int freq, char c, node a, node b)
{
    node n = pool + n_nodes++;
    if (freq)
        n->c = c, n->freq = freq;
    else
    {
        n->left = a, n->right = b;
        n->freq = a->freq + b->freq;
    }
    return n;
}

void qinsert(node n)
{

    int j, i = qend++;
    while ((j = i / 2))
    {

        if (q[j]->freq <= n->freq)
            break;
        q[i] = q[j], i = j;
    }
    q[i] = n;
}


node qremove()
{
    int i, l;
    node n = q[i = 1];

    if (qend < 2)
        return 0;
    qend--;
    while ((l = i * 2) < qend)
    {
        if (l + 1 < qend && q[l + 1]->freq < q[l]->freq)
            l++;
        q[i] = q[l], i = l;
    }
    q[i] = q[qend];
    return n;
}


void build_code(node n, char *s, int len)
{
    static char *out = buf;
    if (n->c)
    {
        s[len] = 0;
        strcpy(out, s);
        code[n->c] = out;
        out += len + 1;
        return;
    }

    s[len] = '0';
    build_code(n->left,  s, len + 1);
    s[len] = '1';
    build_code(n->right, s, len + 1);
}

void init(const char *s)
{
    int i, freq[128] = {0};
    char c[16];


    while (*s)
        freq[(int)*s++]++;


    for (i = 0; i < 128; i++)
    {
        if (freq[i])
            qinsert(new_node(freq[i], i, 0, 0));
    }

    while (qend > 2)
    {
        qinsert(new_node(0, 0, qremove(), qremove()));
    }

    build_code(q[1], c, 0);
}

void EnCoDe(const char *s, char *out,int TokenSayisi)
{
    int i=0;
    while (*s)
    {
        if(i==TokenSayisi)
            break;
        strcpy(out, code[*s]);
        out += strlen(code[*s++]);
        i++;
    }
}
void encode_stream(char *filename,char *Bit,char *Cumle,int BitUzunlugu)
{
    FILE *fout=fopen(filename,"wb");
    int n;
    byte ch;
    byte buf = 0, nbuf = 0;
    byte bitCode[BitUzunlugu];
    printf("\n");
    for(int i=0; i<BitUzunlugu; i++)
    {
        bitCode[i]=Bit[i];
    }
    for(int i=0; i<BitUzunlugu; i++)
    {
        buf |=bitCode[i] << nbuf;
        nbuf++;
        if (nbuf == 8)
        {
            fputc(buf, fout);
            nbuf = buf = 0;
        }
    }
    //fputc(buf,fout);///Bunu yazarsan 1 byte fazla gösteriyor.

}
/*
* iki string'in ilk kaç karakteri özdeþ?
* maksimum limit sayýsý kadar kontrol yapar.
*/
int prefix_match_length(char *s1, char *s2, int limit)
{
    int len = 0;

    while (*s1++ == *s2++ && len < limit)
        len++;

    return len;
}

/*
* memcpy fonksiyonu ile benzer. Byte düzeyinde
* kopyalama yapma garantisi olduðu için, bu
* versiyonu kullanýyoruz.
*/
inline void lz77memcpy(char *s1, char *s2, int size)
{
    while (size--)
        *s1++ = *s2++;
}


/*
* LZ77 ile sýkýþtýrýlacak bir metin alýr.
* token array'i döndürür.
* Eðer numTokens NULL deðilse, token sayýsýný
* numTokens ile gösterilen yere kaydeder.
*/
struct token *encode(char *text, int limit, int *numTokens)
{
    // cap (kapasite) hafýzada kaç tokenlik yer ayýrdýðýmýz.
    int cap = 1 << 3;

    // kaç token oluþturduðumuz.
    int _numTokens = 0;

    // oluþturulacak token
    struct token t;

    // lookahead ve search buffer'larýnýn baþlangýç pozisyonlarý
    char *lookahead, *search;

    // tokenler için yer ayýr.
    struct token *encoded = malloc(cap * sizeof(struct token));

    // token oluþturma döngüsü
    for (lookahead = text; lookahead < text + limit; lookahead++)
    {
        // search buffer'ý lookahead buffer'ýn 31 (OFFSETMASK) karakter
        // gerisine koyuyoruz.
        search = lookahead - OFFSETMASK;

        // search buffer'ýn metnin dýþýna çýkmasýna engel ol.
        if (search < text)
            search = text;

        // search bufferda bulunan en uzun eþleþmenin
        // boyutu
        int max_len = 0;

        // search bufferda bulunan en uzun eþleþmenin
        // pozisyonu
        char *max_match = lookahead;

        // search buffer içinde arama yap.
        for (; search < lookahead; search++)
        {
            int len = prefix_match_length(search, lookahead, LENGTHMASK);

            if (len > max_len)
            {
                max_len = len;
                max_match = search;
            }
        }

        /*
        * ! ÖNEMLÝ !
        * Eðer eþleþmenin içine metnin son karakteri de dahil olmuþsa,
        * tokenin içine bir karakter koyabilmek için, eþleþmeyi kýsaltmamýz
        * gerekiyor.
        */
        if (lookahead + max_len >= text + limit)
        {
            max_len = text + limit - lookahead - 1;
        }


        // bulunan eþleþmeye göre token oluþtur.
        t.offset_len = OFFSETLENGTH(lookahead - max_match, max_len);
        lookahead += max_len;
        t.c = *lookahead;

        // gerekirse, hafýzada yer aç
        if (_numTokens + 1 > cap)
        {
            cap = cap << 1;
            encoded = realloc(encoded, cap * sizeof(struct token));
        }

        // oluþturulan tokeni, array'e kaydet.
        encoded[_numTokens++] = t;
    }

    if (numTokens)
        *numTokens = _numTokens;

    //printf("Encoded ilk yer:%c\t%d\n",(encoded+1)->c,(encoded+5)->offset_len);

    return encoded;
}

// bir dosyanýn tamamýný tek seferde
// okur. Büyük dosyalarý okumak için uygun
// deðildir.
char *file_read(FILE *f, int *size)
{
    char *content;
    fseek(f, 0, SEEK_END);
    *size = ftell(f);
    content = malloc(*size);
    fseek(f, 0, SEEK_SET);
    fread(content, 1, *size, f);
    return content;
}



void compare_algorithm(double dtime, double ltime, int dbyte, int lbyte)
{
    printf("\t\tcompress compare : \n");
    if(dbyte > lbyte)
        printf("\t\t\t LZ77 daha başarılı\n");
    else if(lbyte > dbyte)
        printf("\t\t\t Deflate daha başarılı\n");
    else
        printf("\t\t\t LZ77 ve Deflate aynı oranda başarılıdır");

    printf("\t\ttime compare : \n");
    if(dtime>ltime)
        printf("\t\t\t LZ77 daha başarılı\n");
    else if(ltime>dtime)
        printf("\t\t\t Deflate daha başarılı\n");
    else
        printf("\t\t\t LZ77 ve Deflate aynı oranda daha başarılıdır");
    printf("\n");
}


int main(void)
{
    FILE *f;
    int metin_boyutu = 8, token_sayisi;//Burdaki token sayisi sikiştirilen metnin huffmana göndermesiyle ile birlikte de olmaktadir.
    char *kaynak_metin = "aaaaabcbabbaadcad";
    //char *HuffmanS=256*sizeof(char);//Burdaki boyutu duruma gore ayarla.
    struct token *encoded_metin;

    if (f = fopen("source.txt", "rb"))
    {
        kaynak_metin = file_read(f, &metin_boyutu);
        fclose(f);
    }
    clock_t LZ77_begin = clock();
    printf("Kaynak Metin:%s\n",kaynak_metin);
    encoded_metin = encode(kaynak_metin, metin_boyutu, &token_sayisi);
    printf("Encoded Sonrasi Olusan Token Sayisi:%d\n",token_sayisi);
    char HuffmanS[token_sayisi];
    for(int i=0; i<token_sayisi; i++)
    {
        printf("\t<%d,%d,[%c]>\n ",GETOFFSET((encoded_metin+i)->offset_len),GETLENGTH((encoded_metin+i)->offset_len),(encoded_metin+i)->c);
        if((i+1)%10==0)
            printf("\n");
    }
    printf("\nLZ77 Algoritmasindan sonra yeni metin : \n");
    for(int i=0; i<token_sayisi; i++)
    {
        printf("%c",(encoded_metin+i)->c);
        HuffmanS[i]=(encoded_metin+i)->c;
    }
    clock_t LZ77_end = clock();
    /*printf("\nHuffman Atilcak Metin Kontrolu(Char Array):");
    for(int i=0;i<token_sayisi;i++){
        printf("%c",HuffmanS[i]);
    }*/
    printf("\n");
    if (f = fopen("encoded.txt", "wb"))
    {
        fwrite(encoded_metin, sizeof(struct token), token_sayisi, f);
        fclose(f);
    }

    printf("Orjinal Boyut: %d, Encode Boyutu: %d", metin_boyutu, token_sayisi * sizeof(struct token));
    printf("\n**************************\n");
    //const char *deneme="Ali emayad.Toputiv ktpoumy bsld.";///Burasi lz77 sonucu gelmis metini str adli pointera copy yapaiyorum.
    clock_t Huff_begin = clock();
    const char *str=(char*)malloc((token_sayisi*8)*sizeof(char));///Su anda burada lz77 sonrası şeklinde deniyorum.Bunu sonradan boyutunu degistirön gerekebilir.
    char buf[token_sayisi*8];//encoded kadar burada tutuluyor.
    strcpy(str,HuffmanS);
    init(str);
    for (int i = 0; i < 10024; i++)
        if (code[i])
            printf("'%c': %s\n", i, code[i]);

    //printf("Huffman Kaynak Metin:%s\n\n",str);
    EnCoDe(str, buf,token_sayisi);
    //printf("Huffman encoded:%s\n", buf);
    int n=strlen(buf);//kac bit ile ifade edildiðini bulduk.
    printf("Bit uzunlugu:%d\n",n);
    /*struct EnCode *encoding;
    for(int i=0;i<n;i++){
        (encoding+i)->BitKodu=buf[i];
    }*/
    encode_stream("DeflateEncode.txt",buf,str,n);
    clock_t Huff_end = clock();
    double LZ77_time = (double) (LZ77_end - LZ77_begin) / CLOCKS_PER_SEC ;
    double HUFF_time = (double)(Huff_end - Huff_begin) / CLOCKS_PER_SEC ;
    int DefByte = n/8;
    int LZ77Byte = token_sayisi * sizeof(struct token);
    double DEF_time  = HUFF_time+ LZ77_time;
    printf("\t\t##########\tRESULT\t\t##########\n\n");
    //printf("\t\tHuffman execution time : %f [seconds]\n",HUFF_time);
    printf("\t\tMetnin Orjinal Boyutu : %d byte\n",metin_boyutu);
    printf("\t\tDeflate Algoritmasi sonucu olusan metnin boyutu : %d byte\n",DefByte);///Buraya duruma gore +1 yapicaksin.
    printf("\t\tDeflate execution time : %f [seconds]\n",DEF_time);
    printf("\t\tLZ77 Algoritmasi sonucu olusan metnin boyutu : %d byte\n",LZ77Byte);
    printf("\t\tLZ77 execution time : %F [seconds]\n\n",LZ77_time);
    compare_algorithm(DEF_time, LZ77_time, DefByte, LZ77Byte );
    printf("\t\t##########\tRESULT\t\t##########\n\n");

    return 0;
}
