# Raspberry Pi BASIC
Praca inżynierska
Autorzy:
 - Aleksandra Kosińska
 - Cezary Stajszczyk

Promotor:
 - Piotr Polesiuk

---
## Spis treści
 0. Wstęp
 1. Podstawy projektu
    - 1.1 Proces bootowania Raspberry Pi
    - 1.2 Budowa projektu
    - 1.3 Przygotowanie karty pamięci
 2. BASIC
    - 2.1 Sesja (Ola)
    - 2.2 Parser (Czarek)
    - 2.3 Interpreter (Czarek)
    - 2.4 Ewaluator (Ola)
 3. Pamięć
    - 3.1 Organizacja pamięci (Czarek)
    - 3.2 Zarządzanie pamięcią (Ola)
    - 3.3 MMU (Czarek)
    - 3.4 DMA (Ola)
 4. Komunikacja
    - 4.1 UART (Czarek)
    - 4.2 Klawiatura (Czarek)
    - 4.3 HDMI (Ola)
    - 4.4 Karta SD (Czarek)
 5. Podsumowanie
 6. Bibliografia

---
## Wstęp
 **Dlaczego temat taki, a nie inny?** Bo retroinformatyka jest zajebista i programowanie low level też jest zajebiste i w ogóle my jesteśmy zajebiści dlatego wybraliśmy sobie zajebisty temat. I tak się całkowitym przypadkiem złożyło, że obydwoje posiadamy mini komputerki Raspberry Pi, więc czemu by ich nie użyć. Poza tym użycie takiej płytki daje pewną istotną przewagę nad innymi słabszymi mikroprocesorami jak na przykład Arduino - obsługa HDMI i karty SD jest łatwiejsza (nie wspominając już o tym, że sam procesor jest dużo wydajniejszy, więc można pisać trochę gorszy kod XD).


---
## Rozdział 1: Podstawy projektu (do przeorganiozwania)
Raspberry Pi to nazwa serii komputerów jednopłytkowych stworzonych przez brytyjską organizację charytatywną, której celem jest edukowanie ludzi w zakresie informatyki i ułatwianie dostępu do edukacji informatycznej. Oryginalnie Raspberry Pi zostało wyprodukowane z myślą o języku Python, jednak w prezentowanej pracy obraliśmy zupełnie inne podejście do użytkowania tego urządzenia. Skupiliśmy się na programowaniu niskopoziomowym - tworząc kod w języku C i Asembler który działa na sprzęcie bezpośrednio, bez żadnej podstawowej abstrakcji, takiej jak system operacyjny.

 Mogłoby się wydawać, że obsługa Raspberry Pi jest bardzo prosta – jednak pomimo domysłów – przy charakterze opisanego projektu wcale tak nie jest. Przed pierwszym uruchomieniem Raspberry Pi należy utworzyć odpowiednie pliki oraz zainstalować potrzebne narzędzia, dzięki którym Raspberry Pi poprawie się zbootuje.
Z powodu różnic w architekturze procesorów pomiędzy Raspberry Pi a komputerach, na których tworzony był kod, pracę należy rozpocząć od znalezienia odpowiedniego kompilatora skrośnego (cross-compiler). Ma on za zadanie wygenerowanie kodu na naszych maszynach, który będzie wykonywalny na platformie Raspberry Pi. W prezentowanej pracy wykorzystany został cross-kompilator aarch64-linux-gnu.
Po zainstalowaniu wybranego kompilatora można następnie przystąpić do tworzenia pierwszych niezbędnych plików:

- `start.elf` – plik, którego przykład można znaleźć w instancji systemu operacyjnego Raspbian, odpowiada za poprawne bootowanie pliku .img;
- `kernel8.img` – plik opracowanego mini-systemu. Ósemka dodana na końcu nazwy oznacza, że jest to obraz 64-bitowy. Zostanie on domyślnie załadowany pod adres 0x8000 (32KB);
- `config.txt` – Raspberry Pi nie posiada BIOS’u, zamiast tego wstępną konfigurację dla bootloadera zapisać należy w pliku tekstowym;
- `armstub.bin` – plik istotny szczególnie dla opisywanego projektu, w celu możliwości wspierania obydwu wersji Raspberry Pi: zarówno wersji 3B jak i 4B. Domyślnie płytki te bootują się do różnych poziomów wyjątków (ang. exception levels), co jest dość problematyczne. Własny armstub pozwala na ograniczenie obszerności i ujednolicenie kodu w systemie. Jest to dodatkowa część bootloadera ładowana pod adres 0x0, która wykonywana jest przed kodem z kernel8.img;
- `bootcode.bin` – kod bootloadera odpowiedzialny za załadowanie wszystkich pozostałych plików oraz ustawienie odpowiedniej początkowej konfiguracji. Plik został pozyskany z systemu operacyjnego Raspbian.

 Pliki Makefile to prosty sposób na przeprowadzenie kompilacji kodu. Szczególnie przydatny jest gdy potrzebne są określone rozszerzenia pliku wykonywalnego. Jego struktura przy kompilacji skrośnej nie różni się znacznie od kompilacji programów wewnętrznych komputera. Na początku tworzone są pliki obiektowe z kodu napisanego w Asemblerze oraz w języku C. Drugim krokiem jest wykorzystanie plików obiektowych oraz autorskiego linker’a do utworzenia wykonywalnego oraz linkowalnego pliku z rozszerzeniem .elf. Ostatecznie z wcześniej wymienionego .elf’a powstaje binarny obraz kernel8.img.

Po skompilowaniu plików należy je przenieść na odpowiednio sformatowaną kartę pamięci. Pierwsza partycja na karcie powinna mieć typ FAT32 z włączona flagą bootowania. Ta jedna partycja byłaby wystarczająca do uruchomienia Raspberry Pi, jednak na użytek opisanego projektu konieczne będzie stworzenie drugiej partycji przechowującej dane. To zagadnienie rozwinięte zostanie w następnych rozdziałach. Aby mieć pewność, ze karta SD zostanie poprawnie sformatowana użyliśmy programu RaspberryPi Imager, który udostępniony jest na stronie internetowej komputera.

Gotową kartę SD z wymienionymi wyżej plikami należy umieścić w dedykowanym miejscu na RaspberryPi. Teraz gdy komputer zostanie uruchomiony, przejdzie on procedurę bootowania. Po pomyślnym zakończeniu tego procesu zacznie być wykonywany kod z początku pliku kernel8.img. W przypadku opisywanego systemu, jest to zawartość pliku boot.S, która zgodnie z instrukcjami zawartymi w linker.ld umieszczana jest na samym początku obrazu. Ostatecznie kod Asemblera z pliku boot.S ustawi kolejno podstawowe informacje tj., wspomniany wcześniej poziom wyjątku, włączenie Floating-Point Unit oraz zawieszenie wszystkich drugorzędnych rdzeni procesora. Po zakończeniu wymienionych czynności wywołana zostanie funkcja main().

Dopiero tak przygotowane Raspberry Pi pozwala na rozpoczęcie rozwoju docelowego projektu.
### 1.1 Proces bootowania RaspberryPi

### 1.2 Budowa projektu

### 1.3 Przygotowanie karty pamięci


---
## Rozdział 2: BASIC
**Malinowy BASIC** (nazwa robocza) nie jest wiernym odworowaniem żadnego z już istniejących dwóch milionów dialektów BASICa, ale oczywiście daje gamę funkcjonalności umożliwiających napisanie w nim dowolnego programu. Wiernie za to trzyma się uciążliwości jakie wynikają z konieczności numerowania każdej linii i przepisywania całych instrukcji w przypadku popełnienia choćby jednej literówki :clown_face:
Ale po kolei. Cały kod odpowiedziany za obsługę BASICa można podzielić na trzy główne moduły: `session`, `parser`, `interpreter` oraz `evaluator`.

### 2.1 Sesja
 Objaśnienie tego modułu wypadałoby zacząć od zdefiniowania co właściwie rozumiemy przez "sesję". Sesją nazywać będziemy wszystkie dane związane z programem i konieczne do jego uruchomienia - czyli instrukcje, zmienne, funkcje, a także informacje o stanie programu i ewentualne kody błędów. Struktura sesji jest chyba najważniejszą strukturą w całym naszym BASICu. W tym module oprócz samej struktury znajdują się funkcje umożliwiające operowanie na niej - dodwanie zmiennych, instrukcji, operacje na stosach, a także uruchamianie całego programu.

### 2.2 Parser
 Jest to moduł mniejszy i prostszy od pozostałych, ale zdecydowanie nie mniej ważny. Odpowiada on za przerobienie ciągu znaków będęcego instrukcją na tokeny zrozumiałe dla interpretera. Parsowanie dzieje się u nas "w locie", to znaczy instrukcje w sesji przechowujemy jako ciągi znaków tak jak wpisał je użytkownik, a dopiero podczas wykonywanie programu funkcja `get_next_token()` odczytuje początek instrukcji i zwraca token odpowiadający jej prefixowi. Jest to może i wolniejsze rozwiązania, ale za to zużywa więcej pamięci.

### 2.3 Interpreter
 Konstrukcja tego języka programowania jest na tyle prosta, że już po pierwszym tokenie można jednoznacznie stwierdzić z jaką instruckją mamy do czynienia. Pierwszy token jest odczytywany, a następnie pozostała część ciągu znaków przekazywana jest do funkcji specyficznej dla konkretnej instrukcji. Efektem wykoania instrukcji mogą być dwie rzeczy: wydrukowanie jakichś znaków na standardowym wyjściu lub zmodyfikowanie strukctury sesji poprzez wywołanie odpowiedniej funkcji z modułu `session` (na przykład instruckja `LET X = 42` doda do środowiska zmienną o nazwie `X` typu `INTEGER` z przypisaną do niej wartością `42`).
### 2.4 Evaluator
 Moduł odpowiedzialny za obliczanie warości wyrażeń arytmetyczno logicznych. Użyty algorytm jest szalenie ciekawy, ale ja go nie znam więc go nie opiszę.

Szczegółowy opis wszystkich dostępnych instruckji dostępny będzie na githubie pod [tym linkiem](https://www.youtube.com/watch?v=xvFZjo5PgG0&ab_channel=Duran), jak go w końcu spiszemy.


---
## Rozdział 3: Pamięć (do przeorganizowania)
**Zarządzanie pamięcią** programująć embedded jest dużo bardziej złożone, z kilku powodów. Po pierwsze: pamięci jest mniej więc trzeba ją oszczędnie dysponować - w przypadku RPi3B jest to tylko 1GB pamięci RAM (chociaż w porównaniu do takiego Commodore 64, jest to fch00j). Po drugie: co może nawet bardziej uciążliwe  szczególnie przy dużych projektach - nie ma żadnej ochrony pamięci i można sobie pisać gdzie tylko dusza zapragnie, więc trzeba szczególnie uważać. W przypadku Malinowej Płytki zaskakujące (i uciążliwe) okazało się to, że nawet pisanie pod NULL pointer (adres 0) jest legalne, przez co bugi, które na zwykłym komputerze wyskoczyłyby od razu tutaj były trudniejsze do znalezienia. I w końcu po trzecie, nie ma nawet sterty i alokatora pamięci, do czego jesteśmy przyzwyczajeni, bo jak to tak żyć bez sterty? Wszystko trzeba sobie wyrzeźbić samodzielnie.
Nasz alokator pamięci jest bardzo prosty - bazuje na zwykłej liście łączonej, z headerem i footerem zajmującymi po 4 bajty i przechowującymi dane o rozmiarze bloku oraz o tym, czy jest zaalokowany. Taki algorytm okazał się zupełnie wystarczający jak na nasze potrzeby - piękno tkwi w prostocie! Ponieważ, jak zostało już wcześniej wspomniane, oszczędność miejsca w przypadku programowania embedded jest bardzo istotna, wszelkie wycieki pamięci są bardzo nieporządane. Ale o ile w przypadku programowania na przykład na takim laptopie możemy sobie odpalić wspaniałego Valgrinda, który nam wszystko przetestuje, tutaj nie mamy takich dobrodzejstw. Dlatego do modułu odpowiedzialnego za zarządzanie pamięcią dopisaliśmy własną funkcję, która na wzór Valgrinda pomagała nam przy testowania i dbaniu o zwalnianie wszystkich zasobów. Funkcja działała bardzo prosto: po wydaniu polecenia `SESSEND` aktualna sesja zostaje zakończona i wszystkie zasoby powinny zostać zwolnione. W tym momencie Lickitung :tongue: zagląda do metadanych sterty i sprawdza, czy istnieją jakieś zaalokowane bloki - jeśli tak, zgłasza błąd.
Inną funkcją bardzo przydatną podczas debugowania było drukowania na ekranie mapy pamięci. Funkcjonalość ta dostępna jest przy użyciu meta-instrukcji (ale fancy nazwa) `MEM`.
![](https://hackmd.io/_uploads/Syh0Caqj3.png =256x)

Mała lekcja z języka C: jeżeli mamy strukturę zawierającą kilka elementów o różnych rozmiarach uporządkowanych w kolejności od najmniejszego do największego, to jak pozbyć się automatycznego wyrównania tych elementów tak, by struktura zajmowała jak najmniej miejsca? Należy użyć `__attribute__((__packed__))`. Niestety, w domyślnym stanie procesor nie wspierał operacji na tak niewyrównanych danych. Nie wspierał również z jakiegoś powodu inicjalizacji struktur z użyciem konstruckji compound literals. I tym razem ten problem pakowania nie wynikał z chęci oszczędności miejsca - spakowany struktury są po prostu potrzebne do obsługi pewnych peryferiów, na przykład karty SD. Rozwiązaniem tego problemu było aktywowanie Memory Management Unit (MMU). A niestety, samo aktywowanie MMU wymaga również uruchomienia mechnizmu mapowania pamięci fizycznej na wirtualną. Całe szczęście u nas nie wielu procesów użytkownika wymagająchy wirtualizacji pamięci i wystarczyło nam tylko jedno mapowanie 1:1. Więc mamy piękną tablicę stron, która mapuje adresy na takie same adresy, co wygląda głupawo ale jest niezbędne.

### 3.1 Organizacja pamięci
**(tu będzie śliczny obrazek przedstawiajcy mapę pamięci)**

### 3.2 Zarządzanie pamięcią

### 3.3 MMU

### 3.4 DMA


---
## Rozdział 4: Komunikacja
**Komunikacja ze światem** jest ważna tak samo u komputerów jak i u ludzi. Bo po co komu komputer, z którym nie można sobie porozmawiać? Ale tutaj znowu - o ile w przypadku klasycznego komputera z systemem operacyjnym mielibyśmy wszystko za darmo, tutaj wszystko musimy sobie wyrzeźbić sami. Ale ponieważ super z nas rzeźbierze, w kwestii urządzeń wejścia-wyjścia oferujemy całkiem sporo:

### 4.1 UART
 Podstawowy sposób komunikacji z naszym systemem. Prosty i niezawodny interfejs. Raspberry Pi udostępnia dwie wersje - UART i MiniUART. Różnią się one nie wiadomo czym. Co ważne, obydwa udostępniają przerwania, na których opiera się nasz system inputu. Zdecydowaliśmy się na wybór wersji mini, poniważ małe jest piękne. Ale o ile podczas tworzenia oprogramowania i debugowania UART sprawdział się znakomicie, to powiedzmy sobie jasno: jeżeli do obsługi komputera potrzebuijesz drugiego komputera, to coś jest nie tak.

### 4.2 Zewnętrzna klawiatura
 Wybór interfejsu PS2 podyktowyany był przede wszystkim jego prostotą - dwa przewody (zegar i dane) podpięte bezpośrednio do pinów GPIO na płytce, na pinie zegara włączone przerwanie aktywowane zboczem malejącym (tak na 86%). Dzięki dobrym decyzjom projektowym podjętym już na etapie implementacji UARTa dołożenie tego źródła inputu było prosze niż zawody w wyciskaniu na klatę z przedszkolakami. Drugim argumentem za wybraniem takiej właśnie klawiatury jest to, że pasuje do klimatu retro, a to przecież super istotne. Co prawda mamy na płytce złącza USB, ale to już chyba wyższa szkoła odczytywania danych. Jest jednak pewien haczyk - klawiatury pracują z napięciem 5V, a złącza na płyce 3.3V. Więc o ile nie chcemy usmażyć sobie płytki to mamy dwie opcje. Możemy podłączyć klawiaturą pod 3.3V i liczyć, że będzie działać, albo zastosować konwerter poziomów logicznych, co jest jednak lepszym rozwiązaniem.
 
### 4.3 HDMI
 No to już nie brzmi tak retro jak klawiatura na PS2. I możnaby się bawić w pisanie kodu generującego obraz po kablu kompozytowym podpiętym pod któreś z wielu wolnych pinów (może kiedyś), ale po pierwsze - kto ma jeszcze monitor z takim wejściem? Byłoby to raczej niewygodne. A po drugie - na Malince dostępny jest interfejs do w miarę łatwej obsługi wbudowanego w płytkę złącza HDMI. Interfejsem tym jest Mailbox. Po odpowiednim skonfigurowaniu obsługa jest dziecinnie prosta: mamy w pamięci giga tablicę (zwaną frame bufferem) wielkości 4$\times$liczba pikseli i piszemy sobie wartości pikseli w formacie ARGB pod odpowiednie adresy. Rozwiązanie to ma jednak istotną wadę: nawet w przypadku rozdzielczości tak niewielkiej jak 320$\times$200 pikseli, jeżeli zajdzie potrzeba odświeżenia całego ekranu jest to strasznie wooolne. Z pomocą przychodzi Direct Memory Access (DMA) - po aktywacji nie musimy się przejmować prędkością. Musimy się tylko przejmować tym, że z jakiegoś powodu na RPi3 działa to tylko wtedy, gdy struktura DMA będzie przechowywana pod adresem 0x0 - pierwszy raz widzę sytuacje, gdy trzeba pisać pod nullpointer. Co prawda, pod ten adres ładowany jest nasz `armstub`, ale po zbootowaniu nie jest on już potrzebny, więc można go nadpisać.

### 4.4 Karta SD
 No fajnie, że możemy sobie pisać jakieś programy, ale jeszcze fajniej byłoby móc je zapisywać gdzieś indziej niż na kartce i później je wczytywać. Tutaj cała na biało chodzi karta SD. Poza pierwszą partycją do bootowania wystarczy utworzyć drugą o wielkości przynajmniej 1GB i typie jakimkolwiek bo i tak żaden sensowny nie jest obsługiwany. Nasz program utworzy na niej własny system plików, gdzie w pierwszym bloku przechowywać będzie tablicję metadanych plików (max. 32 pliki), a kolejny ~1GB podzieli na równe części i przypisze odpowiednio każdemu wejściu w tablicy. Pierwsze wesjćie jest specjalne i zawsze zajęte.

### 4.5 GPIO
 W zasadzie pewnym urządzeniem wejścia/wyjścia są też same piny GPIO, mamy fajny moduł do ich obsługi, chociaż my je tutaj raczej traktujemy jako pośredników. Ale jeżeli ktoś chciałby sobie na przykład pomrugać LEDami to można, jak najbardziej.


---
## Podsumowanie
No nie wiem jak wy, ale ja się bawiłem zajebiście.


---
## Bibliografia
 - [Raspberry Pi documentation](https://www.raspberrypi.com/documentation/)
 - [BCM 2837 datasheet (RPi3)](https://www.google.com/url?sa=t&rct=j&q=&esrc=s&source=web&cd=&ved=2ahUKEwj0kum6iKD2AhVIx4sKHRyCDZcQFnoECCIQAQ&url=https%3A%2F%2Fcs140e.sergio.bz%2Fdocs%2FBCM2837-ARM-Peripherals.pdf&usg=AOvVaw0ujRgTqIAwAxaY9a03v-on)
 - [BCM 2711 datasheet (RPi4)](https://datasheets.raspberrypi.com/bcm2711/bcm2711-peripherals.pdf)
 - [101 BASIC computer games](https://annarchive.com/files/Basic_Computer_Games_Microcomputer_Edition.pdf)
 - [Low Level Devel](https://github.com/rockytriton/LLD/tree/main)
 - [uBASIC: a really simple BASIC interpreter](https://github.com/adamdunkels/ubasic)
 - [SD and SDIO](http://yannik520.github.io/sdio.html)
 - Mikrokomputery: Commodore 64 ~ Bohdan Frelek
