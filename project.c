#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#define VAL 20
#define INFINITY 2147483647

typedef struct {		    /* struttura per l'array della TopK */
	int index;
	int weight;
    bool occupied;
} struct_t;

typedef struct node_ {		/* struttura per i nodi della lista della TopK */
	int index;
	int weight;
	struct node_ *next;
} node_t;


void DataInsert(int *d, int *k);	/* sottoprogramma per acquisire dall'utente i dati d e k */
void UserChoice(int *choiceValue);	/* sottoprogramma per chiedere all'utente se inserire un nuovo grafo oppure visualizzare la TopK */
int Dijkstra(int *graph, int *distance, bool *seen, int *d);    /* sottoprogramma che opera sul grafo e ne restituisce la somma dei cammini minimi da 0 verso tutti gli altri nodi */
void getGraphAdjM(int *graph, int *d);			/* sottoprogramma che acquisisce il grafo */
struct_t* keepOrderArray(struct_t *tmpTopKArray, int *iter, int *totalWeight, int *k);    /* per inserire i nodi in ordine nell'array che contiene la TopK */
void printTopKArray(struct_t *tmpTopKArray, int *k);  /* per visualizzare la TopK con array */
node_t* keepOrderList(node_t *list, int *index, int *weight, int *k);   /* per mantenere la lista della TopK ordinata */
void printTopKList(node_t *list, int *k);   /* per visualizzare la TopK con lista */
node_t* addHead(node_t* list, int *index, int *weight); /* per aggiungere in testa nella lista */
node_t* delTopK(node_t *list);  /* per eliminare la lista */

int main(){
	int d, k, i, iter, totalWeight, choiceValue;
    int *graph, *distance;
    struct_t *tmpTopKArray=NULL;
    node_t *tmpTopKList=NULL;

	iter=0;

	DataInsert(&d, &k);

	UserChoice(&choiceValue);
	
	/* allocazione dinamica dello spazio per contenere i dati dei grafi e dati temporanei per l'elaborazione del calcolo dei cammini minimi */
    graph=malloc((d*d)*sizeof(int));
	distance=malloc(d*sizeof(int));
    bool seen[d];

    if(k<VAL){
        tmpTopKArray=(struct_t*)calloc(k, sizeof(struct_t));
        for(i=0; i<k; i++){
            tmpTopKArray[i].weight=0;
            tmpTopKArray[i].index=0;
            tmpTopKArray[i].occupied=false;
        }
    } 

	while((choiceValue==1 || choiceValue==2)){		/* itero finché non ho in input qualcosa di diverso da AggiungiGrafo o TopK */
		if(choiceValue==1){							/* se choiceValue == 1, l'utente ha scelto di aggiungere un grafo, quindi lo acquisisco e ne calcolo le somme dei cammini minimi */
			getGraphAdjM(graph, &d);
			totalWeight=Dijkstra(graph, distance, seen, &d);
            
            if(k<VAL){					  /* inserisco l'indice del grafo e il suo costo nella TopK temporanea */
			    tmpTopKArray=keepOrderArray(tmpTopKArray, &iter, &totalWeight, &k);	
            } else {
                tmpTopKList=keepOrderList(tmpTopKList, &iter, &totalWeight, &k);
            }
            
            iter++;
		}

		if(choiceValue==2){              /* se choiceValue == 2, l'utente ha scelto di visualizzare la TopK: la stampo */
            if(k<VAL){						
			    printTopKArray(tmpTopKArray, &k);
            } else {
                printTopKList(tmpTopKList, &k);
            }
        }
		
        UserChoice(&choiceValue);				/* viene richiesta l'operazione da svolgere */

	}

	/* deallocazione dello spazio */
	free(graph);
    free(distance);
    free(tmpTopKArray);
    delTopK(tmpTopKList);

	return 0;
}



/* SOTTOPROGRAMMI */


/* SOTTOPROGRAMMI PER LA GESTIONE DELLA TOPK */

/* sottoprogramma per mantenere la TopK in ordine crescente  (array) */
struct_t* keepOrderArray(struct_t *tmpTopKArray, int *iter, int *totalWeight, int *k){
    int i, j;
    
    for(i=0; i<*k; i++){
        if(tmpTopKArray[i].occupied==true){
            if(*totalWeight<tmpTopKArray[i].weight){
                for(j=*k-2; j>=i; j=j-1){
                    if(tmpTopKArray[j].occupied==true){
                        tmpTopKArray[j+1].weight=tmpTopKArray[j].weight;
                        tmpTopKArray[j+1].index=tmpTopKArray[j].index;
                        tmpTopKArray[j+1].occupied=true;
                    }
                }
                tmpTopKArray[i].weight=*totalWeight;
                tmpTopKArray[i].index=*iter;
                tmpTopKArray[i].occupied=true;
                return tmpTopKArray;
            }
        } else {
            tmpTopKArray[i].weight=*totalWeight;
            tmpTopKArray[i].index=*iter;
            tmpTopKArray[i].occupied=true;
            return tmpTopKArray;
        }
    }
    return tmpTopKArray;
}


/* sottoprogramma che stampa a video la TopK (array) */
void printTopKArray(struct_t *tmpTopKArray, int *k){
    int i;

    for(i=0; i<*k && tmpTopKArray[i].occupied; i++){
        if(i!=0){
            putchar_unlocked(32);
        }
        printf("%d", tmpTopKArray[i].index);
    }
    putchar_unlocked(10);
}


/* sottoprogramma che stampa gli indici dei grafi appartenenti alla TopK (liste) */
void printTopKList(node_t *list, int *k){
	int i;
	for(i=0; list!=NULL && i<*k; i++, list=list->next){
		printf("%d", list->index);
		if(i!=*k-1 && list->next!=NULL)
			putchar(32);
	}
	putchar(10);
}


/* sottoprogramma che aggiunge un nuovo nodo in testa alla lista */
node_t* addHead(node_t* list, int *index, int *weight){
	node_t *tmp;
	tmp=malloc(sizeof(node_t));
	if(tmp!=NULL){
		tmp->index=*index;
		tmp->weight=*weight;
		tmp->next=list;
		list=tmp;
	}
	return list;
}


/* sottoprogramma per tenere la TopK temporanea in ordine crescente (liste) */
node_t* keepOrderList(node_t *list, int *index, int *weight, int *k){
	node_t *tmp;

	if(!list || list->weight>*weight)
		return addHead(list, index, weight);
	list->next=keepOrderList(list->next, index, weight, k);

	/* se il numero di grafi che aggiungo è maggiore di k, elimino quelli che sicuramente resteranno fuori dalla TopK */
	if(*index>=(*k-1)){		
		for(tmp=list; tmp->next!=NULL; tmp=tmp->next);
		tmp=tmp->next;
		free(tmp);
	}
	return list;
}

/* sottoprogramma per eliminare la lista */
node_t* delTopK(node_t *list){
	node_t *tmp;
	while(list!=NULL){
		tmp=list;
		list=list->next;
		free(tmp);
	}
	return NULL;
}


/* SOTTOPROGRAMMI PER L'ACQUISIZIONE DEGLI INPUT */

/* sottoprogramma che acquisisce il numero di nodi dei grafi e la lunghezza della TopK */
void DataInsert(int *d, int *k){
	char c;
	
	*d=0;
	c=getchar_unlocked();
	while(c!=' '){
		*d=*d*10+(c-'0');		/* acquisisco cifra per cifra */
		c=getchar_unlocked();
	}

	*k=0;
	c=getchar_unlocked();
	while(c!='\n'){
		*k=*k*10+(c-'0');
		c=getchar_unlocked();
	}
}


/* sottoprogramma che chiede all'utente se inserire un grafo oppure visualizzare la TopK */
void UserChoice(int *choiceValue){
	char c;

	c=getchar_unlocked();
	if(c=='A'){
		while(c!='\n'){
			c=getchar_unlocked();
		}
		*choiceValue=1;
	} else if(c=='T'){
		while(c!='\n'){
			c=getchar_unlocked();
		}
		*choiceValue=2;
	} else {
		*choiceValue=3;
	}

}


/* sottoprogramma per acquisire la matrice di adiacenza del grafo */
void getGraphAdjM(int *graph, int *d){
	int i, j;
	char c; 
	
	for(i=0; i<*d; i++){
		for(j=0; j<*d; j++){
			graph[i*(*d)+j]=0;
			c=getchar_unlocked();
			while(c!=',' && c!='\n'){
				graph[i*(*d)+j]=graph[i*(*d)+j]*10+(c-'0');
				c=getchar_unlocked();
			}
		}
	}

}


/* SOTTOPROGRAMMI PER L'ELABORAZIONE DEL GRAFO */

/* dijkstra -> per calcolare i cammini minimi dal nodo 0 */
int Dijkstra(int *graph, int *distance, bool *seen, int *d) {
    int i, iter, totalWeight, tmpMinimum, next;
    next=0;

    /* inizializzo il vettore delle distanze e il vettore seen che è vero se un nodo è stato ispezionato, falso se non lo è stato */
    for (i=0; i<*d; i++){
        if(graph[i]==0)     /* se il valore della matrice di adiacenza del grafo è 0, significa che i due nodi non sono connessi -> la distanza è infinita */
            distance[i]=INFINITY;
        else
            distance[i]=graph[i];
        seen[i]=false;
    }
	/* inizializzo per il nodo 0 che è la sorgente */
    distance[0]=0;
    seen[0]=true;


    /*calcolo dei cammini minimi */
    for (iter=1; iter<*d-1; iter++) {
        tmpMinimum=INFINITY;
        for (i=0; i<*d; i++){
            if (distance[i]<tmpMinimum && !seen[i]){
                tmpMinimum=distance[i];
                next=i;
            }
        }

        /* verifico l'esistenza di un percorso più breve */
        seen[next]=true;
        for(i=0; i<*d; i++){
            if (!seen[i]){
                if(graph[next*(*d)+i]==0)   /* se il valore della matrice di adiacenza del grafo è 0, significa che i due nodi non sono connessi -> la distanza è infinita */
                    graph[next*(*d)+i]=INFINITY;
                if(tmpMinimum<distance[i]-graph[next*(*d)+i])
                    distance[i]=tmpMinimum+graph[next*(*d)+i];
            }
        }
    }

    /* calcolo la somma dei percorsi da 0 verso ciascun altro nodo */
	totalWeight=0;
	for(i=0; i<*d; i++){
		if(distance[i]==INFINITY)
			distance[i]=0;
		totalWeight=totalWeight+distance[i];
	}

    return totalWeight;
}
