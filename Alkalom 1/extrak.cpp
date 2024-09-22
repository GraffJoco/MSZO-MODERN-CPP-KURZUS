#include <print>
#include <vector>
#include <memory>
#include <functional>

using namespace std;

int main() {
    // Lambda egy osztály, ez kihasználható templateknél
    {
        // Példa: memória hívása/törlése malloccal
        const auto freemem = [](const auto* ptr) {
            free((void*)ptr);
        };

        unique_ptr<int[]> sima_okosptr = make_unique<int[]>(5);

        unique_ptr<int[], function<void(int*)>> functionnal((int*)malloc(5 * sizeof(int)), freemem);

        // decltype használata std::function kikerülésére
        unique_ptr<int, decltype(freemem)> decltypepal((int*)malloc(5 * sizeof(int)));

        // lambda beágyazás haszna: kisebb típus, pointerezés fordításkor megy
        println("sima unique_ptr: {}", sizeof(sima_okosptr));
        println("std::function<> hasznalataval: {}", sizeof(functionnal));
        println("decltype hasznalataval: {}", sizeof(decltypepal));
    }

    // Tömb foglalása stacken alloca használatával
    // delete erre nincs, okospointer felesleges
    {
        int* stacktomb = (int*)alloca(5 * sizeof(int));

        // Probléma: konstruktor-destruktor osztálynál nem hívódik
        // megoldás? new-nak lehet megadni, hova foglaljon

        struct osztaly {
            int adat;

            osztaly(int adat = 5) : adat(adat) {
                println("Letrehozva");
            }

            // Sosem hívódik
            ~osztaly() {
                println("Torolve");
            }
        };

        osztaly* simastack = (osztaly*)alloca(sizeof(osztaly) * 5);
        // Konstruktor hívódik, destruktor nem
        osztaly* newosstack = new (alloca(sizeof(osztaly) * 5)) osztaly[5];
    }
}
