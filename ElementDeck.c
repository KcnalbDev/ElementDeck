#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>

// --- Definições de Tipos ---

typedef enum
{
    Fogo,
    Agua,
    Eletricidade,
    Terra,
    Vento,
    Luz,
    Escuridao,
    SemAfinidade

} Elemento;

typedef struct heroi
{
    char nome[50];
    int HP,MP,forca;
    int HP_MAX,MP_MAX;
    float percAfinidade;
    Elemento afinidade;
    Elemento fraqueza;
    float (*especial)(struct heroi *,struct heroi *);
    char *nomeEspecial;

} Heroi;

// --- Implementação das Funções ---

Heroi *createHeroi()
{
  Heroi *heroi = (Heroi *) malloc(sizeof(Heroi));
  if (heroi == NULL) exit(EXIT_FAILURE);
  return heroi;
}

Heroi **getListaHerois(int tam)
{
    Heroi **herois = (Heroi **) malloc(sizeof(Heroi *) * tam);
    if (herois == NULL)
        exit(EXIT_FAILURE);
    for (int i=0;i<tam;i++)
        herois[i] = createHeroi();
    return herois;
}

void liberaHeroi(Heroi *heroi)
{
  if (heroi == NULL)
    return;
  free(heroi->nomeEspecial);
  free(heroi);
}

void liberaHerois(Heroi **herois,int tam)
{
    for (int i=0;i<tam;i++)
        liberaHeroi(herois[i]);
    free(herois);
}

int getRandInt(int min,int max)
{
    return (rand() % (max - min + 1)) + min;
}

float especialGenerico(Heroi *ataque, Heroi *defesa)
 {
    printf("-> '%s' usou seu poder especial '%s' (Bonus: 5.0)!\n", ataque->nome, ataque->nomeEspecial);
    return 5.0;
}

void inicializaHeroi(Heroi *h, char *nome, int hp, int mp, int forca, Elemento afinidade, Elemento fraqueza, float percAfinidade, float (*especial)(Heroi *, Heroi *), char *nomeEspecial)
{
    if (h == NULL) return;

    
    strncpy(h->nome, nome, 49);
    h->nome[49] = '\0';
    h->HP = hp; h->HP_MAX = hp;
    h->MP = mp; h->MP_MAX = mp;
    h->forca = forca;
    h->afinidade = afinidade;
    h->fraqueza = fraqueza;
    h->percAfinidade = percAfinidade;
    h->especial = especial;
    h->nomeEspecial = strdup(nomeEspecial);
    if (h->nomeEspecial == NULL) exit(EXIT_FAILURE);
}

void imprimirHeroi(Heroi *heroi)
{
    if (heroi == NULL)
        return;
    printf("-> %s (HP: %d/%d, MP: %d/%d, Forca: %d, Af: %d, Fraq: %d)\n",
           heroi->nome, heroi->HP, heroi->HP_MAX, heroi->MP, heroi->MP_MAX, heroi->forca, heroi->afinidade, heroi->fraqueza);
}

void imprimirHerois(Heroi **herois, int tam)
{
    printf("--- LISTA DE HEROIS (%d) ---\n", tam);
    for (int i = 0; i < tam; i++)
        imprimirHeroi(herois[i]);
    printf("---------------------------\n");
}

int comparaNome(Heroi *a,Heroi *b)
{
    return strcmp(a->nome,b->nome);
}

int buscaHeroi(Heroi **herois,Heroi *heroi,int ini,int fim,int (*comparar)(Heroi*,Heroi*))
{
    if (ini > fim)
        return -1;
    int meio = (fim + ini) / 2;
    int comp = comparar(herois[meio],heroi);

    if (comp == 0)
        return meio;
    else if (comp < 0)
        return buscaHeroi(herois,heroi,meio+1,fim,comparar);
    else
        return buscaHeroi(herois,heroi,ini,meio-1,comparar);
}

void buscaElemento(Heroi **herois, int tam, Heroi ***elemHerois, int *tamH, Elemento elem)
{
    int count = 0;
    for (int i = 0; i < tam; i++)
    {
        if (herois[i]->afinidade == elem) count++;
    }

    if (count == 0)
    {
        *elemHerois = NULL; *tamH = 0;
        return;
    }

    *elemHerois = (Heroi **) malloc(sizeof(Heroi *) * count);
    if (*elemHerois == NULL)
        exit(EXIT_FAILURE);

    int j = 0;
    for (int i = 0; i < tam; i++)
        {
            if (herois[i]->afinidade == elem)
            {
                (*elemHerois)[j] = herois[i];
                j++;
            }
        }
    *tamH = count;
}


void applyDamage(Heroi *ataque,Heroi *defesa)
{
    if (ataque->MP == 0 || ataque->afinidade==SemAfinidade)
    {
        int dano = ataque->forca;
        int novo_hp = defesa->HP - dano;

        if (novo_hp >= 0)
        {
            defesa->HP = novo_hp;
        }
        else
        {
            defesa->HP = 0;
        }
    }
    else
    {
        float danoBase = 0;
        float danoEspecial = 1.0;

        if (getRandInt(0,9)==0)
        {
            danoEspecial += ataque->especial(ataque,defesa);
            printf("-> '%s' aplicou especial\n",ataque->nome);
        }

        int descMP = (int)(ataque->MP_MAX * (getRandInt(10,20)/100.0));
        ataque->MP = (ataque->MP - descMP) >= 0 ? (ataque->MP - descMP) : 0;

        danoBase += ataque->forca;
        danoBase *= 1 + (ataque->percAfinidade/100.0);

        if (ataque->afinidade==defesa->fraqueza || defesa->fraqueza==SemAfinidade){
            danoBase *= 1.3;
        }
        danoBase *= danoEspecial;

        defesa->HP = (defesa->HP - (int)danoBase) >= 0 ? (defesa->HP - (int)danoBase) : 0;
    }
    printf("'%s'[HP:%i,MP:%i] atacou '%s'[HP:%i,MP:%i]\n",
            ataque->nome,ataque->HP,ataque->MP,defesa->nome,defesa->HP,defesa->MP);
}

Heroi *battleHeroes(Heroi *a,Heroi *b)
{
    printf("\n--- INICIO DA BATALHA: %s vs %s ---\n", a->nome, b->nome);
    Heroi *ataque = NULL;
    Heroi *defesa = NULL;

    while (a->HP > 0 && b->HP > 0)
    {
        switch (getRandInt(0,1))
        {
            case 0:
                ataque = a; defesa = b;
                break;
            case 1:
                ataque = b; defesa = a;
                break;
        }

        applyDamage(ataque,defesa);
        if (defesa->HP <= 0)
        break;

        Heroi *temp_ataque = (ataque == a) ? b : a;
        Heroi *temp_defesa = (ataque == a) ? a : b;

        if (temp_ataque->HP > 0)
            applyDamage(temp_ataque, temp_defesa);
    }
    if (a->HP > 0)
        {
            printf("--- FIM DA BATALHA: VENCEDOR: %s ---\n", a->nome);
            return a;
        }
    else if (b->HP > 0)
        {
            printf("--- FIM DA BATALHA: VENCEDOR: %s ---\n", b->nome);
            return b;
        }
    else
        {
            printf("--- FIM DA BATALHA: EMPATE ---\n");
            return NULL;
        }
}

void curarHerois(Heroi **herois, int tam)
{
    for (int i = 0; i < tam; i++)
    {
        herois[i]->HP = herois[i]->HP_MAX;
        herois[i]->MP = herois[i]->MP_MAX;
    }
    printf("\nTodos os heróis foram curados (HP e MP restaurados).\n");
}


int main()
{
    srand(time(NULL));
    const int TAM_HEROIS = 5;
    Heroi **herois = NULL;
    int opcao;

    herois = getListaHerois(TAM_HEROIS);

    inicializaHeroi(herois[0], "Kai (Fogo)", 100, 50, 15, Fogo, Agua, 30.0, especialGenerico, "Inferno Flamejante");
    inicializaHeroi(herois[1], "Maya (Agua)", 90, 60, 12, Agua, Eletricidade, 35.0, especialGenerico, "Tsunami");
    inicializaHeroi(herois[2], "Zora (Lutador)", 110, 0, 25, SemAfinidade, SemAfinidade, 0.0, especialGenerico, "Soco Final");
    inicializaHeroi(herois[3], "Lyra (Luz)", 80, 70, 10, Luz, Escuridao, 40.0, especialGenerico, "Flashback");
    inicializaHeroi(herois[4], "Nix (Terra)", 120, 40, 20, Terra, Vento, 25.0, especialGenerico, "Terremoto");

    // --- LOOP PRINCIPAL DO MENU INTERATIVO ---
     printf("\n================ MENU DE BATALHA ================\n");
    printf("0. Sair e Liberar Memoria\n");
    printf("1. Visualizar status de todos os Herois\n");
    printf("2. Iniciar Batalha (Escolher 2 Herois)\n");
    printf("3. Buscar Herois por Elemento\n");
    printf("4. Curar todos os Herois (HP e MP ao MAX)\n");
    printf("=================================================\n");
    printf("Escolha uma opcao: ");
    scanf("%d", &opcao);
    printf("\n");

    while (opcao != 0)
    {
        switch (opcao)
        {
             case 0:
                printf("\nSaindo do programa...\n");
                break;

            case 1:
                imprimirHerois(herois, TAM_HEROIS);
                break;

            case 2:
            {
                int indexA, indexB;
                imprimirHerois(herois, TAM_HEROIS);
                printf("\nDigite o INDICE do primeiro Heroi (1 a %d): ", TAM_HEROIS);
                scanf("%d", &indexA);
                printf("Digite o INDICE do segundo Heroi (1 a %d): ", TAM_HEROIS);
                scanf("%d", &indexB);

                if (indexA > 0 && indexA <= TAM_HEROIS && indexB > 0 && indexB <= TAM_HEROIS && indexA != indexB)
                    {
                        Heroi *h1 = herois[indexA - 1];
                        Heroi *h2 = herois[indexB - 1];

                        if (h1->HP < h1->HP_MAX || h2->HP < h2->HP_MAX)
                        {
                            printf("Heróis estão feridos. Curando antes da batalha...\n");
                            curarHerois(herois, TAM_HEROIS);
                        }

                        Heroi *vencedor = battleHeroes(h1, h2);
                        printf("\n--- RESULTADO FINAL ---\n");
                        printf("Vencedor: %s\n", vencedor != NULL ? vencedor->nome : "Empate");

                    }
                    else
                    {
                        printf("Seleção inválida.\n");
                    }
                break;
            }

            case 3:
            {
                int elemBusca;
                printf("Elementos: (1:Fogo, 2:Agua, 3:Eletricidade, 4:Terra, 5:Vento, 6:Luz, 7:Escuridao, 8:SemAfinidade)\n");
                printf("Qual elemento buscar (1 a 8): ");
                scanf("%d", &elemBusca);

                elemBusca = elemBusca - 1;
                if (elemBusca >= 0 && elemBusca <= 7)
                {
                    Heroi **elemHerois = NULL;
                    int tamH = 0;

                    buscaElemento(herois, TAM_HEROIS, &elemHerois, &tamH, (Elemento)elemBusca);

                    if (tamH > 0)
                    {
                        printf("\n-> Heróis de Afinidade %d encontrados:\n", elemBusca);
                        imprimirHerois(elemHerois, tamH);
                        free(elemHerois);
                    }
                    else
                    {
                        printf("Nenhum herói encontrado com essa afinidade.\n");
                    }
                }
                else
                {
                    printf("Elemento inválido.\n");
                }
                break;
            }

            case 4:
                curarHerois(herois, TAM_HEROIS);
                break;

            default:
                printf("Opção inválida.\n");
        }
        printf("\n");
        printf("0. Sair e Liberar Memoria\n");
        printf("1. Visualizar status de todos os Herois\n");
        printf("2. Iniciar Batalha (Escolher 2 Herois)\n");
        printf("3. Buscar Herois por Elemento\n");
        printf("4. Curar todos os Herois (HP e MP ao MAX)\n");
        printf("=================================================\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        printf("\n");
    }

    // --- LIBERAÇÃO DE MEMÓRIA ---
    liberaHerois(herois, TAM_HEROIS);
    printf("Memoria liberada com sucesso.\n");

    return 0;
}
