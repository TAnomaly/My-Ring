# IP Webcam Kurulum Rehberi - Türkçe

## 📱 Android Telefonda IP Webcam Kurulumu

### Adım 1: Uygulama İndirme
1. **Google Play Store**'u açın
2. **"IP Webcam"** arayın (Pavel Khlebovich tarafından)
3. Uygulamayı **indirin ve kurun**

### Adım 2: Temel Ayarlar
1. IP Webcam uygulamasını açın
2. **Video ayarları**:
   - **Çözünürlük**: 640x480 veya 1280x720 (önerilen)
   - **Kalite**: %70-80 arası
   - **FPS**: 15-30 (ağ hızınıza göre)

3. **Ses ayarları** (isteğe bağlı):
   - Ses kaydını açabilirsiniz

### Adım 3: Sunucuyu Başlatma
1. Uygulamanın alt kısmında **"Start server"** butonuna tıklayın
2. Ekranda bir **IP adresi** görünecek, örneğin:
   ```
   http://192.168.1.100:8080
   ```
3. Bu IP adresini **not alın** - bilgisayarda kullanacağız

### Adım 4: Ağ Bağlantısı Kontrolü
- Telefon ve bilgisayarın **aynı WiFi ağında** olduğundan emin olun
- Router ayarlarında port engeli olmadığını kontrol edin

## 💻 Bilgisayarda Kullanım

### Uygulamamızı Çalıştırma

#### Windows'ta:
```cmd
# Build klasörüne gidin
cd build

# Varsayılan IP ile çalıştır
IPWebcamProcessor.exe

# Kendi IP adresinizle çalıştır
IPWebcamProcessor.exe "http://192.168.1.100:8080/video"
```

#### Linux/macOS'ta:
```bash
# Build klasörüne gidin
cd build

# Varsayılan IP ile çalıştır
./IPWebcamProcessor

# Kendi IP adresinizle çalıştır
./IPWebcamProcessor "http://192.168.1.100:8080/video"
```

## 🎮 Kullanım Kılavuzu

### Tuş Komutları
| Tuş | İşlev | Açıklama |
|-----|-------|----------|
| **G** | Gri Tonlama | Görüntüyü siyah-beyaz yapar |
| **B** | Bulanıklık | Gaussian blur filtresi uygular |
| **E** | Kenar Tespiti | Canny algoritması ile kenarları bulur |
| **H** | HSV Renk | HSV renk uzayına dönüştürür |
| **M** | Hareket Tespiti | Kareler arası hareket algılar |
| **+/-** | Parlaklık/Kontrast | Görüntü ayarlarını değiştirir |
| **R** | Sıfırla | Orijinal görüntüye döner |
| **S** | Kaydet | Anlık görüntü kaydeder |
| **ESC** | Çıkış | Uygulamayı kapatır |

### Ekran Bilgileri
- **FPS**: Saniyedeki kare sayısı
- **Filter**: Aktif filtre adı
- **Bağlantı Durumu**: Yeşil=Bağlı, Kırmızı=Bağlantı Yok
- **İstatistikler**: İşlenen kare sayısı ve çalışma süresi

## 🔧 Sorun Giderme

### Bağlantı Sorunları

#### "Cannot connect to webcam" Hatası
```
Çözüm:
1. IP adresini kontrol edin
2. Telefon ve PC aynı ağda mı?
3. IP Webcam uygulaması çalışıyor mu?
4. Farklı port deneyin (8080, 8888, 4747)
```

#### Yavaş/Kesikli Video
```
Çözüm:
1. Video kalitesini düşürün (%50-60)
2. FPS'i azaltın (15-20)
3. Çözünürlüğü düşürün (640x480)
4. WiFi sinyalini güçlendirin
```

#### Frequent Disconnections
```
Çözüm:
1. Telefonu şarja takın
2. Uygulamayı arka plan kısıtlamalarından muaf tutun
3. WiFi uyku modunu kapatın
4. Router'a daha yakın olun
```

## 📱 Telefon Ayarları (Android)

### Enerji Tasarrufu
1. **Ayarlar** > **Batarya** > **Uygulama Optimizasyonu**
2. **IP Webcam**'ı bulun ve **optimizasyon dışı** bırakın

### WiFi Ayarları
1. **Ayarlar** > **WiFi** > **Gelişmiş**
2. **WiFi'yi uyku modunda aç tut** seçeneğini etkinleştirin

### Kamera İzinleri
1. **Ayarlar** > **Uygulamalar** > **IP Webcam**
2. **İzinler** > **Kamera** > **İzin Ver**

## 🌐 Test URL'leri

### Tarayıcıdan Test
Telefondaki IP adresini bilgisayar tarayıcısında açın:
```
http://192.168.1.100:8080
```

Canlı video görmek için:
```
http://192.168.1.100:8080/video
```

### Farklı Port Örnekleri
```bash
# Port 8080 (varsayılan)
./IPWebcamProcessor "http://192.168.1.100:8080/video"

# Port 8888
./IPWebcamProcessor "http://192.168.1.100:8888/video"

# Port 4747
./IPWebcamProcessor "http://192.168.1.100:4747/video"
```

## 🎯 Optimizasyon İpuçları

### En İyi Performans İçin
1. **Çözünürlük**: 640x480 (hızlı ağ için 1280x720)
2. **FPS**: 20-25 
3. **Kalite**: %70-80
4. **Codec**: H.264 (mevcut ise)

### Düşük Latency İçin
1. Buffer boyutunu minimize edin (kod zaten optimize)
2. Kablolu bağlantı kullanın (mümkünse)
3. 5GHz WiFi tercih edin (2.4GHz yerine)
4. Diğer ağ trafiğini azaltın

## 📋 Kontrol Listesi

### Kurulum Öncesi
- [ ] Telefon ve PC aynı WiFi ağında
- [ ] IP Webcam uygulaması kurulu
- [ ] Kamera izni verilmiş
- [ ] C++ uygulaması derlenmiş

### İlk Çalıştırma
- [ ] IP Webcam sunucusu başlatıldı
- [ ] IP adresi not alındı
- [ ] Tarayıcıdan test edildi
- [ ] C++ uygulaması IP ile çalıştırıldı

### Sorun Giderme
- [ ] Bağlantı durumu kontrolü
- [ ] Ağ ayarları kontrolü
- [ ] Firewall/antivirus kontrolü
- [ ] Farklı port denemesi

## 💡 İlave Özellikler

### Çoklu Kamera
Birden fazla telefonla çoklu kamera kurabilirsiniz:
```bash
# 1. telefon
./IPWebcamProcessor "http://192.168.1.100:8080/video"

# 2. telefon (farklı terminal)
./IPWebcamProcessor "http://192.168.1.101:8080/video"
```

### Kayıt ve Analiz
- **S tuşu** ile anlık görüntü kaydedin
- Kaydedilen dosyalar timestamp ile adlandırılır
- Motion detection ile hareket analizi yapın

### Performans İzleme
- FPS sayacını takip edin
- Bağlantı durumunu gözlemleyin
- Çıkışta performans özeti görün

---

## 🆘 Hızlı Yardım

**Problem**: Bağlanamıyorum
**Çözüm**: IP adresi doğru mu? Aynı ağda mısınız?

**Problem**: Video yavaş
**Çözüm**: Kaliteyi düşürün, FPS azaltın

**Problem**: Sürekli kopuyor
**Çözüm**: Enerji tasarrufunu kapatın, WiFi güçlendirin

---

**Not**: Bu rehber Android IP Webcam uygulaması için yazılmıştır. iOS için benzer uygulamalar mevcuttur. 