/*
 * Autor: Nicolescu Roxana-Elena
 * File: README
 */



**************************Grupare fisiere dupa continut dintr-un director*******************

	* Inceputul e basically cum am facut listarea elementelor unui director;
	* Am un std::map, unde cheia este size-ul unui fisier si valoarea std::vector ce 
		contine numele fisierelor cu size-ul respectiv;
		* Pe masura ce dau de un fisier nou, ii calculez size-ul si vad daca in map
			exista cheia respectiva;
			* Daca exista, adaug in lista de la cheia respectiva numele fisierului,
			* Daca nu, adaug o noua cheie cu valoarea o lista cu singur element, numele fisierelui gasit.
	* Dupa ce am format map-ul, parcurg fiecare cheie;
		* Daca dau de o lista cu un singur element, afisez numele fisierului;
		* Daca lista are mai multe elemente, compar doua cate doua fisierele;
			* Pentru asta, folosesc doua vectori:
				* unul in care tin indicii fiecarui fisier, fisierele identice vor avea acelasi index;
				* altul in care tin fiecare Handle pentru fisierele deschide
			* Compar elementele apeland o functie care imi citeste cate un byte din fiecare fisier primit ca parametru,
				si in functie de comparatie, returneaza true sau false;
		* Dupa ce compar fisierele doua cate doua, parcurg vectorul index;
		* Pentru a afisa frumos ma folosesc de un map, asemanator cu cel pentru size;
			* Cheia este indexul si valoarea e o lista cu numele fisierelor identice,
		* Parcug map-ul pentru a afisa fisierele identice.



	 





			 