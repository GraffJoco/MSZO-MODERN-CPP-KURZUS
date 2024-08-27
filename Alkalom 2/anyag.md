# Tartalom

- [BEVEZETÉS](#bevezetés)
- [SABLONOK A C++-BAN](#sablonok-a-c-ban)
  - [SABLONOK ALAPJAI (C++98)](#sablonok-alapjai)
  - [SABLONOK SZABÁLYOZÁSA (C++20)](#sablonok-szabályozása)

# Bevezetés

A kurzus 2. alkalmának kereteiben a C++ fordítóját fogjuk kihasználni, aminek hála a munka jelentős részét a fordító automatikusan csinálja helyettünk, legyen az kódgenerálás, függvények futtatása fordításkor, vagy sablon alapján típus/függvénygenerálás.

# Sablonok a C++-ban

## Sablonok alapjai

A sablonok a C++ egyik legrégebbi, de legerősebb újítása: egyszer megírt osztályt fordításkor ismert paraméterek alapján lehet különböző formákba építeni. Erre egy példa, amit múltkor használtunk már, a `std::vector<T>`. A `<T>` egy sablon, ami szerint fordításkor adhatunk neki egy $T$ típust, amit behelyettesít, és ha nem okoz ez semmilyen errort, akkor ezt akárhány különböző típussal meg lehet ismételni úgy, hogy a `vector` osztályt csak egyszer írták le a készítők.

Sablont osztályból a `template` kulcsszóval lehet csinálni, ahol relációs jelek között a fordítási paramétereket lehet definiálni:

~~~C++
#include <print>

// typename vagy class: típus, amit be lehet helyettesíteni
template <typename T>
struct vektor {
    // T-t fordításkor behelyettesíti egy adott típusra
    T x, y;

    vektor(T x, T y) : x(x), y(y) {}

    T hossz() {
        return T(sqrt(x * x + y * y));
    }
};

int main() {
    // T helyére akármit be lehet helyettesíteni
    vektor<int> egyik(1, 2);
    vektor<double> masik(-9.8, 3.14159);

    std::print("Egyik hossza: {}\nMasik hossza: {}", egyik.hossz(), masik.hossz());
}
~~~

Ezt az eszközt gyakran típusok behelyettesítésére használják, de akár sima értékeket (pl.: `int`, `double`) is lehet felhasználni. Ennek egy nagy haszna az, hogy fordításkor ismert mérettel lehet statikus tömböket használni az osztályban, nem kell pointerezni.

~~~C++
#include <print>
#include <string>

template <typename T, size_t Meret>
class tomb {
private:
    // Mivel Meretet ismerjük fordításkor, ezért
    // lehet statikus tömböt csinálni vele
    T data[Meret];

public:
    // nem kell Meretet tárolni, elég egy függvényt
    // írni, ami visszaadja az értékét
    inline const size_t meret() const { return Meret; }

    // Referenciát ad vissza, hogy a tömbérték módosítható legyen
    inline T& operator[](size_t index) {
        return data[index];
    }

    void print() const {
        std::print("[");
        for (size_t i = 0; i < Meret - 1; i++) {
            std::print("{}, ", data[i]);
        }
        if (Meret != 0) {
            std::print("{}", data[Meret - 1]);
        }
        std::print("]");
    }
};

int main() {
    tomb<double, 3> egyik;
    egyik[0] = 1.2;
    egyik[1] = 6.9;
    egyik[2] = -0.24;

    egyik.print();
    std::println("\nA tomb {} byte, ami {} elem * {} byte per elem", sizeof(egyik), 3, sizeof(double));

    tomb<std::string, 2> masik;

    masik[0] = "Ezt is lehet";
    masik[1] = "Akarmilyen tipus hasznalhato";

    masik.print();
}
~~~

Függvényekből is lehet sablont csinálni, ugyanazzal a szintaxissal. Az újabb szabványok egyik előnye, hogy gyakran a paraméterekből a sablonparamétereket is kitalálja.  
Az előző példát ezek alapján lehet bővíteni:

~~~C++
#include <print>
#include <string>

template <typename T, size_t Meret>
class tomb {
private:
    // Mivel Meretet ismerjük fordításkor, ezért
    // lehet statikus tömböt csinálni vele
    T data[Meret];

public:
    // nem kell Meretet tárolni, elég egy függvényt
    // írni, ami visszaadja az értékét
    inline const size_t meret() const { return Meret; }

    inline T& operator[](size_t index) {
        return data[index];
    }

    inline const T& operator[](size_t index) const {
        return data[index];
    }

    void print() const {
        std::print("[");
        for (size_t i = 0; i < Meret - 1; i++) {
            std::print("{}, ", data[i]);
        }
        if (Meret != 0) {
            std::print("{}", data[Meret - 1]);
        }
        std::print("]");
    }
};

template <typename T, size_t Meret>
T szumma(const tomb<T, Meret>& adatok) {
    T sum{};

    for (size_t i = 0; i < Meret; i++) {
        sum += adatok[i];
    }

    return sum;
}

int main() {
    tomb<double, 3> egyik;
    egyik[0] = 1.2;
    egyik[1] = 6.9;
    egyik[2] = -0.24;

    egyik.print();
    std::println("\nA tomb {} byte, ami {} elem * {} byte per elem", sizeof(egyik), 3, sizeof(double));

    // nem kötelező <double, 3> kiírása, paraméterből ki tudja találni a fordító
    std::println("Szumma: {}", szumma(egyik));

    tomb<std::string, 2> masik;

    masik[0] = "Ezt is lehet";
    masik[1] = "Akarmilyen tipus hasznalhato";

    masik.print();
    std::println("\n{}", szumma<std::string>(masik));
}
~~~

## Sablonok szabályozása

Az előző példák bemutatták a sablonok előnyeit, viszont van egy hátrány: ha a behelyettesített típus errorhoz vezet, gyakran oldal méretű errorokat ad, amiket lehetetlen debuggolni.

Például, ha az előző fejezet `vektor` példaosztályába stringet helyettesítünk be, a következő errort kapjuk:

![Legértelmesebb C++ error](kepek/cpp_sablon_error.png)

Ez csak azt mondja, hogy a `vektor<std::string>::hossz()` függvényt nem tudta lefordítani, nem pedig azt, hogy $T$-nek számértéknek kellene lennie. Ebben az esetben még valamennyire érthető, de nagyobb osztályok és többréteges behelyettesítéseknél már nem.  

Azért, hogy meg lehessenek szabva szabályok a behelyettesítéseknek, több szabvályban különböző új dolgokat adtak a nyelvhez, hogy - amennyiben valami ezeket a szabályokat megszegi - érthető errorokat kapjon a fejlesztő. Ezek közül a `requires` kulcsszóról, és a `<type_traits>` könyvtárról lesz szó.  

A `requires` kulcsszó alapján egyes osztályoknak, függvényeknek vagy tagfüggvényeknek egy ifhez hasonló módon lehet megadni, hogy a paraméterek alapján szabad-e lefordítani.  

A `<type_traits>` könyvtár engedélyezi, hogy osztályok tulajdonságait igen/nem értékké alakítsa a fordító.

Például, egy vektornál azt akarjuk, hogy az adatok, amiket tárol, számok legyenek. Tehát ha azt mondjuk, hogy $T$ vagy egész (`is_integral_v`) vagy lebegőpontos (`is_floating_point_v`) típus:

~~~C++
template <typename T>
requires (std::is_floating_point_v<T> || std::is_integral_v<T>)
class vektor2 {
    // ...
};
~~~

Ha ebbe az osztályba stringet helyettesítünk be, akkor egy egyszerűbb errort kapunk, hogy a vektor korlátainak nem felel meg a `T = std::string` behelyettesítés:

![requires kulcsszó által generált error](kepek/requires_error.png)

Amennyiben ilyen feltételeket többször használunk, vagy az errort olvashatóbb formába akarjuk alakítani, akkor tudunk egy `concept`-et írni. Ezek a kifejezések nem csak a `type_traits` tulajdonságait tudják felhasználni, hanem ha adsz nekik egy példakódot, akkor az alapján, hogy egy adott típust behelyettesítve ad-e errort a mintakód, tudja eldönteni, hogy megfelelnek-e a template paraméterek.  
Például, ha egy számot akarunk kifejezésben leírni, a következő módokon is csinálhatjuk:

~~~C++
template <typename T>
concept number = requires (T egyikSzam, T masikSzam) {
    double(egyikSzam); // double-re alakithatonak kell lennie
    // Alap matematikai műveleteknek meg kell felelnie
    // ha ezek egyike nem fordul le, akkor T nem felel meg a szám koncepciónak
    egyikSzam + masikSzam;
    egyikSzam - masikSzam;
    egyikSzam * masikSzam;
    egyikSzam / masikSzam;
    egyikSzam == masikSzam;
    egyikSzam > masikSzam;
    egyikSzam >= masikSzam;
    egyikSzam < masikSzam;
    egyikSzam <= masikSzam;
};

template <number T>
struct vektor {
    // ...
};
~~~

Ha most próbálunk egy hibás típust (ezesetben stringet) behelyettesíteni, akkor a fordító jelzi nekünk, hogy a `std::string` nem felel meg a `number` kifejezésnek:

![concept által generált C++ error](kepek/concept_error.png)

A `concept` kifejezéseknek egyik előnye, hogy függvényekben elég `auto` kulcssó elé elhelyezni, és `requires` rész nélkül is garantálja a fordító, hogy minden korrekt:

~~~C++
auto abszolut(number auto szam) {
    if (szam < 0) {
        return 0 - szam;
    } else {
        return szam;
    }
}

int main() {
    std::print("{} {}", abszolut(-1.0), abszolut(2));
}
~~~
