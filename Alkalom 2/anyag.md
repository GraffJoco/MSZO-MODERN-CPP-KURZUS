# Tartalom

- [BEVEZETÉS](#bevezetés)
- [SABLONOK A C++-BAN](#sablonok-a-c-ban)
    - [SABLONOK ALAPJAI](#sablonok-alapjai)

# Bevezetés

A kurzus 2. alkalmának kereteiben a C++ fordítóját fogjuk kihasználni, aminek hála a munka jelentős részét a fordító automatikusan csinálja helyettünk, legyen az kódgenerálás, függvények futtatása fordításkor, vagy sablon alapján típus/függvénygenerálás.

# Sablonok a C++-ban

## Sablonok alapjai

A sablonok a C++ egyik legrégebbi, de legerősebb újítása: egyszer megírt osztályt fordításkor ismert paraméterek alapján lehet különböző formákba építeni. Erre egy példa, amit múltkor használtunk már, a `std::vector<T>`. A `<T>` egy sablon, ami szerint fordításkor adhatunk neki egy $T$ típust, amit behelyettesít, és ha nem okoz ez semmilyen errort, akkor ezt akárhány különböző típussal meg lehet ismételni úgy, hogy a `vector` osztályt csak egyszer írták le a készítők.

Sablont osztályból a `template` kulcsszóval lehet csinálni, ahol relációs jelek között a fordítási paramétereket lehet definiálni:

```C++
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
    vektor<int> egyik(1, 2);
    vektor<double> masik(-9.8, 3.14159);

    std::print("Egyik hossza: {}\nMasik hossza: {}", egyik.hossz(), masik.hossz());
}
```
