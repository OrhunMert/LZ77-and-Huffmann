[DOSYA SIKIŞTIRMA UYGULAMASI]

*********PROJE ÖZETİ*********

>Bu proje source.txt isimli bir dosya içerisindeki metni okuyup bir char pointer'ına atılır.
>Daha sonrasında bu okunan metni attığımız char pointer'ı LZ77 encode ve Deflate encode methodlarına gönderilerek encode edilir.
>LZ77 ve Deflate encode methodlarından sonra LZ77 sıkıştırılmış metni encoded.txt , Deflate sıkıştırılmış metni ise DeflateEncode.txt isimli dosyaya yazdırılır.
>Son olarak LZ77 ve Deflate algoritmaları sıkıştırma ve süre bakımından karşılaştırılır
NOT1 > source.txt 'nın boyut sınırı vardır ve bu sınır 10kb olarak belirledik
NOT2 > source.txt içeriğinde Klavyeden girilemeyen ve Türkçe karakterler olmamalıdır , örnek olarak : ❝ ❞  ve ‘ ’ ve - ve _ ve benzeri karakterler segmantation core fault hatası verdirir. source.txt dosyasında kullanılan tüm karakterler ascii tablosunda bulunmalıdır, aksi taktirde kod segmantation core fault hatası verir

*********PROJEYİ İNDİRMEK ve PROJE'Yİ ÇALIŞTIRMAK ,TEST ETMEK*********

>1_180201011_180201159.zip içerisindeki 'calistirilabilirKisim- C exe dosyası' isimli klasör içerisindeki a.out dosyasını Linux'ta terminalden .\a.out yazılarak çalıştırılabilir
>1_180201011_180201159.txt dosyasının içerisindeki kodu kopyalayıp yeni bir console uygulaması oluşturup oraya yapıştırarak çalıştırabilirsiniz, yeni oluşturduğunuz console uygulaması klasörüne gidip source.txt isminde bir dosya oluşturup içerisinde metin olmalıdır.
>1_180201011_180201159.zip içerisindeki 1_180201011_180201159 isimli klasör direk orjinal proje dosyasıdır oradan çalıştırabilirsiniz
>.zip dosyasını açmak için 
>Windows'ta çeşitli programlar kullanabilirsiniz 
>Linux'ta ise eğer .zip dosyasında şifre yoksa unzip dosya_adi.zip / eğer .zip dosyasında şifre var ise unzip -P SIFRE dosya_adi.zip ile açabilirsiniz
>Test etmek için source.txt dosyasının içeriğini değiştirip run edebilirsiniz

*********KODUN ÇALIŞMA SÜRESİ ve ÇALIŞMA MANTIĞI*********
>Kodun çalışma mantığı metni oku , önce LZ77 encode metnini oluştur , LZ77 encode metnini Huffman algoritmasında kullanarak Deflate encode metnini oluşturmaktır
>Çalışma süresiyle alakalı source.txt dosyasının boyutu ile doğru orantılı olarak artmaktadır. 
>Kendi bilgisayarımda 857 byte'lık bir source dosyasını 0.0004 s çalıştırmaktadır

*********PROJEYİ HAZIRLAYAN**********
>Abdussamed KILIÇ (180201011)
>Orhun Mert BOZKURT (180201159)



