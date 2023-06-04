1. Każde wyrażnie matematyczne lub logiczne musi być otoczone okrągłymi nawiasami
2. W jedej linii dopuszczalna jest jedna instrukcja (nie można odzielać instrukcji dwukropkiem)
3. Nazwy zmiennych nie mogą zaczynać się słowem kluczowym basica
4. Nieznaczące spacje na początku linii są zabronione
5. Kiedy przesować pos o varname length? Podczas czytania tokenu (żeby być konsekwentnym w funkcji get_next_token) czy podczas czytania nazwy zmiennej?
6. Może funkcja get_next_token() powinna mieć dodatkowy argument *dest, do którego kopiowany będzie tekst tokenu? Przydatne w przypadku zmiennych, funkcji i wyrażeń
7. A może po prostu do kolejnych funkcji przekazywać przesunięty wskaźnik?