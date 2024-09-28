# Tartalom

- [BEVEZETÉS](#bevezetés)
  - [LAMBDA KALKULUS ALAPJAI](#lambda-kalkulus-alapjai)
  - [TISZTA FÜGGVÉNYEK, MUTABILITÁS](#tiszta-függvények-mutabilitás)
- [LAMBDÁK, ALGORITMUSOK A C++-BAN](#lambdák-algoritmusok-a-c-ban)
  - [TISZTA LAMBDA FÜGGVÉNYEK](#tiszta-lambda-függvények)

# Bevezetés

A funkcionál programozás nagyon más, mint az imperatív vagy akár objektum orientált programozás, ezért sajnos itt egy elméleti bevezető is kelleni fog.

## Lambda kalkulus alapjai

A funkcionál programozás az ú.n. lambda kalkulusra épül, aminek a következő a lényege: *minden* matematikai fogalom kifejezhető egyváltozós függvények kombinálásával. Ez a szoftveres világban olyan szinten appikálható, hogy minden program is leírható függvények kreatív kombinálásával.

Lambda kalkulusban a függvényeket egy $\lambda$-val jelölik, utána a paramétereket, pont után pedig a kifejezést:

$$ E \equiv \lambda x .x+5$$  

$$ E(5) \equiv 10 $$

Amennyiben többváltozós függvényünk van, körrizést felhasználva tudunk írni egy olyan egyváltozós függvényt, ami egy másik egyváltozós függvényt ad vissza (>2 paraméternél ezt többször megismételve) egy paramétert behelyettesítve:

$$ F \equiv \lambda x.\lambda y.x+y$$  

$$ F(5)(7) \equiv (\lambda x. \lambda y. x + y)(5)(7) \equiv (\lambda y . 5 + y)(7) \equiv 5 + 7 = 12 $$

C++-ban ez a következő módon nézhet ki:

~~~C++
const auto F = [](const int x) {
    return [=](const int y) {
        return x + y;
    };
};
~~~

Habár C++-ban a körrizést szerencsére nem kell használni (más nyelvekben, pl.: Haskellben kötelező), ebből egy olyan fontos következtetést lehet levonni, amire szükségünk lesz: **első osztályúak a függvények, tehát paraméterként, értékként és függvények eredményeként is kezelhetőek**. Aki már használt C++ lambdákat (amik innen kapták a nevüket) az `<algorithm>` könyvtárral STL tárolókon, az már ezt a tulajdonságot kihasználta.

## Tiszta függvények, mutabilitás

A funk. prog. egyik legfontosabb fogalma, és alapja a tiszta függvények használata. A tiszta függvények a következő szabályoknak felelnek meg:

- A függvény kimenete csak a paraméterek ismeretével mindig meghatározható (nem olvas külső értékeket a paraméterein kívül)
- Nem módosít semmilyen bemeneti paramétert
- Nincsenek mellékhatásai (nem nyúl globális/külső változókhoz, radikális esetben még I/O művelet is tiltott)

Ha egy függvény tiszta, akkor egyszerű teszteléssel lehet garantálni, hogy stabilan, és mindig úgy csinálja feladatát, ahogyan azt definiáltuk. Természtesen bizonyos bemenetekre vigyázni kell, hogy a lehetséges hibákat, error értékeket detektáljuk, és lekezeljük.  
Amennyiben tud egy függvény sikertelen lenni, error értéket kell visszaadni eredményként, hogy a hívó tudja, hogy a függvény nem mindig sikeres, és lekezelje ezt az esetet. Erre volt hasznos az első alkalomról a `std::optional<T>` típusunk, és ennek a bővítése, a `std::expected<T, U>`, amiben konkrét adatként tudjuk pontosítani az errort.

A hibák elkerülésére a "mutabilis" értékeket is ajánlott minimalizálni. Magyarra fordítva, próbálj mindent konstansokkal megoldani, mert tudod, ha a kezdeti értéke jó, akkor mindenhol is az. Emiatt már egyes nyelvekben (pl.: Rustban) azt kell külön kulcsszóval jelölni, ha *nem* konstans a változó, és van, ahol csak konstansok léteznek (pl.: Haskell, Gleam). Ez természetesen nem a legjobb megoldás, gyakran kell, hogy a változók értéke *változzon*, de ha lehet, ajánlott konstansokat használni.

# Lambdák, algoritmusok a C++-ban

Ahhoz, hogy a C++-ban tudjuk ezt a tudást alkalmazni, elő kell hozni az [első alkalom](../Alkalom%201/anyag.md) alatt tanult lambdákat.  
A lambda egy funktor (függvényként használható osztály), amit szabadon akárhol létrehozhatunk a következő szintaxissal:

~~~C++
const auto lamdba = [/*megfogott értékek*/](/*paraméterek*/) {
    // függvény elemek
};
~~~

## Tiszta lambda függvények

Ahhoz, hogy tiszta függvényként kezelhessük a lambdánkat, a következő szabályoknak kell megfelelnie:

- Minden változót vagy `=`-vel érték szerint másolunk, vagy ha `&`-vel referenciaként másoljuk, akkor nem módosítjuk értékét, de optimális esetben **nem fogunk meg külső értéket, csak paraméterként**
- A paraméterek vagy sima értékek vagy konstans referenciák

Például:

~~~C++
#include <print>

int main() {
    int val = 5;
    
    // Tiszta fv
    const auto duplaz1 = [](const auto param) {
        return param * 2;
    };

    // Tiszta fv
    const auto duplaz2 = [](const auto& param) {
        return param * 2;
    };

    // Nem tiszta, paramétert módosítja
    const auto duplaz3 = [](auto& param) {
        param *= 2;
    };

    // Nem tiszta, külső értéket mutál
    const auto duplaz4 = [&val]() {
        val *= 2;
    };

    // Nem tiszta, de jobb, mint az előző, mert val nem módosul
    const auto duplaz5 = [=]() {
        return val * 2;
    };

    // Technikailag nem tiszta, de konzolba kiírás nem nagy probléma
    const auto duplaz6 = [](const auto& param) {
        const auto ret = param * 2;
        std::println("{}", ret);
        return ret;
    };
}
~~~
