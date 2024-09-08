#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
	#define CLEAR "cls"
#else
	#define CLEAR "clear"
#endif

int fechar=0;  //opcao de fechar o jogo apos zerar
char username[31];   //nome do usuario
char pseudousername[10000]; //variavel de validacao do username
char pseudooptionmainmenu[1000];//variavel de validacao da opcao do menu principal
int optionmainmenu; //opcao do menu principal
char pseudooptionconfigmenu[1000]; //variavel de validacao da opcao do menu de configuracoes
int optionconfigmenu;  //opcao do menu de configuracoes
char pseudodif[1000]; //variavel de validacao da opcao do menu das dificuldades
int dif; //opcao do menu de dificuldades
char pseudozera[1000]; //variavel de validacao da opcao de zerar ranking
char zera; //opcao para zerar o ranking
char pseudoopzerou[1000]; //variavel de validacao da opcao do menu zerou
int opzerou; // opcao do menu zerou
FILE* fd;   //ponteiro para os arquivos das dificuldades
FILE* fp;   //ponteiro para o arquivo do ranking
int dificuldade=1; //dificuldade
int matriz_original[10][10];  //matriz original do arquivo
int matriz_usuario[10][10];  //matriz com as alteracoes do usuario
int somacoluna[10];  //soma das colunas do arquivo
int somalinha[10];  //soma das linhas do arquivo
int progresso[12]; //confere o progresso do jogador, para saber se ele concluiu todas as fases, ou seja, zerou o jogo
int nivel=0; //corresponde a fase dentro de cada dificuldade. Cada dificuldade tem 4 niveis. Como sao 3 dificuldades, o nivel começa em 0 e vai ate 11
int zerou=0; //conferir se o usuario zerou o jogo, que depende se o vetor progresso eh todo constituido de 1
typedef struct{  //struct que armazena os dados de cada jogador do ranking, contendo apenas nome e a pontuacao atual
	char nome[31];  
	int pontuacao;
} ranking;
ranking players[1000];  //vetor de struct com maximo de 1000 para armazenar as informacoes do ranking
int qtdranking;  // variavel utilizada para controlar a quantidade de players no ranking
void nivelcontrol(){  // funcao para ajustar a dificuldade conforme a passagem dos niveis
	if(nivel>=0 && nivel<=3){
		dificuldade=1;
	}
	else if(nivel>=4 && nivel<=7){
		dificuldade=2;
	}
	else if(nivel>=8 && nivel<=11){
		dificuldade=3;
	}
}

void progressocontrol(){ //funcao para preencher o vetor de progresso a medida que o usuario passa das fases
	int i;
	for(i=0; i<12; i++){
		if(nivel==i+1){
			progresso[i]=1;
		}
	}
}

void checkzerou(){ //funcao que confere se o jogador zerou o jogo
	int i;
	zerou=1;
	for(i=0; i<12; i++){
		if(progresso[i]==0){
			zerou=0;
			break;
		}
	}
}

void nameorder(ranking players[], int qtdranking){ // funcao que ordena o ranking por ordem alfabetica
	int i, j;
	for(j = 0; j < qtdranking; j++){
		for(i = 0; i < qtdranking-1; i++){
			if(strcmp(players[i].nome, players[i+1].nome)>0){
				ranking aux = players[i];
				players[i] = players[i+1];
				players[i+1] = aux;
			}
		}
	}
}

void pointsorder(ranking players[], int qtdranking){ // funcao que ordena o ranking por ordem de pontos
	int i, j;
	for(j = 0; j < qtdranking; j++){
		for(i = 0; i < qtdranking-1; i++){
			if(players[i].pontuacao < players[i+1].pontuacao){
				ranking aux = players[i];
				players[i] = players[i+1];
				players[i+1] = aux;
			}
		}
	}
}

void menuranking(){  //funcao que mostra o ranking
	int i;
	fp=fopen("ranking.bin", "rb");
	fread(&qtdranking, sizeof(int), 1, fp);
	if(qtdranking==0 || fp==NULL){
		printf("O ranking esta vazio no momento.\n");
		printf("Digite enter para continuar...");
		getchar();
	}
	else{
		for(i=0; i<qtdranking; i++){
			fread(&players[i], sizeof(ranking), 1, fp);
		}
		printf("\n");
		printf("   RANKING   \n");
		printf("\n");
		for(i=0; i<qtdranking; i++){
			printf("%d) %s - %d\n", i+1, players[i].nome, players[i].pontuacao);
		}
		printf("\n");
		printf("Digite enter para fechar o ranking...");
		getchar();
	}
	fclose(fp);
}

int rank(){  //funcao que organiza todo o ranking em memoria ram e no arquivo
	int i, n=0;
	int jatem=0;
	fp=fopen("ranking.bin", "rb");
	if(fp==NULL){
		qtdranking=0;
		fclose(fp);
		fp=fopen("ranking.bin", "w+b");
		fwrite(&qtdranking, sizeof(int), 1, fp);
		fclose(fp);
	}
	else{
		fread(&qtdranking, sizeof(int), 1, fp);
	}
	for(i=0; i<qtdranking; i++){
		fread(&players[i], sizeof(ranking), 1, fp);
	}
	fclose(fp);
	for(i=0; i<qtdranking; i++){
		if(strcmp(username, players[i].nome)==0){
			jatem=1;
			break;
		}
	}
	if(jatem==1){
		if(dificuldade==1){
			players[i].pontuacao+=50;
		}
		else if(dificuldade==2){
			players[i].pontuacao+=100;
		}
		else{
			players[i].pontuacao+=200;
		}
	}
	else{
		strcpy(players[qtdranking].nome, username);
		if(dificuldade==1){
			players[qtdranking].pontuacao=50;
		}
		else if(dificuldade==2){
			players[qtdranking].pontuacao=100;
		}
		else{
			players[qtdranking].pontuacao=200;
		}
		qtdranking++;
	}
	nameorder(players, qtdranking);
	pointsorder(players, qtdranking);
	fp=fopen("ranking.bin", "wb");
	fwrite(&qtdranking, sizeof(int), 1, fp);
	for(i=0; i<qtdranking; i++){
		fwrite(&players[i], sizeof(ranking), 1, fp);
	}
	fclose(fp);
	return 0;
}

void printmatrizog(){ //funcao que printa na tela a matriz original(do arquivo)
	system(CLEAR);
	int i, j;
	if(dificuldade==1){        //printa matriz iniciante
		fd=fopen("iniciante.txt", "r");
		if(nivel==1){
			fseek(fd, 88, SEEK_SET);
		}
		else if(nivel==2){
			fseek(fd, 176, SEEK_SET);
		}
		else if(nivel==3){
			fseek(fd, 264, SEEK_SET);
		}
		for(i=0; i<4; i++){
			for(j=0; j<4; j++){
				fscanf(fd, "%d", &matriz_original[i][j]);
				matriz_usuario[i][j]=matriz_original[i][j];
			}
		}
		for(i=0; i<4; i++){
			fscanf(fd, "%d", &somacoluna[i]);
		}
		for(i=0; i<4; i++){
			fscanf(fd, "%d", &somalinha[i]);
		}
		printf("    %.2d %.2d %.2d %.2d\n", somacoluna[0], somacoluna[1], somacoluna[2], somacoluna[3]);
		printf("   -------------\n");
		for(i=0; i<4; i++){
			printf("%.2d |", somalinha[i]);
			for(j=0; j<4; j++){
				printf(" %d ", matriz_original[i][j]);
			}
			printf("\n");
		}
	}
	else if(dificuldade==2){   //printa matriz intermediario
		fd=fopen("intermediario.txt", "r");
		if(nivel==5){
			fseek(fd, 180, SEEK_SET);
		}
		else if(nivel==6){
			fseek(fd, 360, SEEK_SET);
		}
		else if(nivel==7){
			fseek(fd, 540, SEEK_SET);
		}
		for(i=0; i<6; i++){
			for(j=0; j<6; j++){
				fscanf(fd, "%d", &matriz_original[i][j]);
				matriz_usuario[i][j]=matriz_original[i][j];
			}
		}
		for(i=0; i<6; i++){
			fscanf(fd, "%d", &somacoluna[i]);
		}
		for(i=0; i<6; i++){
			fscanf(fd, "%d", &somalinha[i]);
		}
		printf("    %.2d %.2d %.2d %.2d %.2d %.2d\n", somacoluna[0], somacoluna[1], somacoluna[2], somacoluna[3], somacoluna[4], somacoluna[5]);
		printf("   -------------------\n");
		for(i=0; i<6; i++){
			printf("%.2d |", somalinha[i]);
			for(j=0; j<6; j++){
				printf(" %d ", matriz_original[i][j]);
			}
			printf("\n");
		}
	}
	else if(dificuldade==3){    //printa matriz avançado
		fd=fopen("avancado.txt", "r");
		if(nivel==9){
			fseek(fd, 238, SEEK_SET);
		}
		else if(nivel==10){
			fseek(fd, 476, SEEK_SET);
		}
		else if(nivel==11){
			fseek(fd, 714, SEEK_SET);
		}
		for(i=0; i<7; i++){
			for(j=0; j<7; j++){
				fscanf(fd, "%d", &matriz_original[i][j]);
				matriz_usuario[i][j]=matriz_original[i][j];
			}
		}
		for(i=0; i<7; i++){
			fscanf(fd, "%d", &somacoluna[i]);
		}
		for(i=0; i<7; i++){
			fscanf(fd, "%d", &somalinha[i]);
		}
		printf("    %.2d %.2d %.2d %.2d %.2d %.2d %.2d\n", somacoluna[0], somacoluna[1], somacoluna[2], somacoluna[3], somacoluna[4], somacoluna[5], somacoluna[6]);
		printf("   ----------------------\n");
		for(i=0; i<7; i++){
			printf("%.2d |", somalinha[i]);
			for(j=0; j<7; j++){
				printf(" %d ", matriz_original[i][j]);
			}
			printf("\n");
		}
	}
}

void printmatrizu(){ //funcao que printa a matriz do usuario, ou seja, com todas as alteracoes 
	int i, j;
	int n;
	if(dificuldade==1){
		n=4;	
	}
	else if(dificuldade==2){  // seleciona o tamanho do n baseado na dificuldade
		n=6;
	}
	else if(dificuldade==3){                                          
		n=7;
	}
	printf("   ");
	for(i=0; i<n; i++){
		if(somacoluna[i]!=0){
			printf(" %.2d", somacoluna[i]);
		}
		else{
			printf("   ");
		}
	}
	printf("\n");
	if(dificuldade==1){
		printf("   -------------\n");
	}
	else if(dificuldade==2){
		printf("   -------------------\n");
	}
	else if(dificuldade==3){                                          
		printf("   ----------------------\n");
	}
	for(i=0; i<n; i++){
		if(somalinha[i]!=0){
			printf("%.2d |", somalinha[i]);
		}
		else{
			printf("   |");
		}
		for(j=0; j<n; j++){
			if(matriz_usuario[i][j]==0){
				printf("   ");
			}
			else{
				printf(" %d ", matriz_usuario[i][j]);
			}
		}
		printf("\n");
	}
}

void jogar(){ //funcao que realiza todas as mecanicas da jogabilidade em si
	int ganhou; // variavel que monitora se o jogador ganhou a fase atual
	int vidas=5; // variavel das vidas
	int linha, coluna; // variaveis que guardam o input do usuario, ou seja, as linhas que colunas para apagar
	char pseudolinha[1000];
	char pseudocoluna[1000];
	int i, j;
	int n, erro, flag=0;
	if(dificuldade==1){
		n=4;
	}
	else if(dificuldade==2){
		n=6;
	}
	else if(dificuldade==3){                                          
		n=7;
	}
	int gabarito[n][n]; //matriz gabarito para conferir se as eliminacoes condizem com a matriz gabarito do arquivo
	int gablinha[n]; //vetor para armazenar a soma das linhas da matrizdo usuario, para verificar se a linha foi fechada
	for(i=0; i<n; i++){
		gablinha[i]=0;
	}
	int gabcoluna[n];  //vetor para armazenar a soma das colunas da matriz do usuario, para verificar se a coluna foi fechada
	for(i=0; i<n; i++){
		gabcoluna[i]=0;  
	}
	while(vidas!=0 && ganhou!=n*2){ //loop da fase em si, que termina quando o jogador perde ou ganha
		system(CLEAR);
		if(flag==0){ //a variavel flag serve para dizer se o jogador acabou de abrir a fase. Isso serve para decidir se a matriz a ser mostrada deve ser a original ou do usuario
			printmatrizog();
			for(i=0; i<n; i++){
				for(j=0; j<n; j++){
					fscanf(fd, "%d", &gabarito[i][j]);
				}
			}
		}
		else{
			printmatrizu();
		}
		erro=0;
		printf("***Voce tem %d vidas***\n", vidas);
		printf("Digite a linha e a coluna do numero que deseja apagar, mestre.\n");
		printf("Linha:");
		fgets(pseudolinha, 1000, stdin);
		int l=strlen(pseudolinha); //tamanho original de pseudolinha
		if(pseudolinha[l-1]=='\n'){
			pseudolinha[l-1]='\0'; //retirando o \n da string por conta do fgets
		}
		if(l>2){
			printf("Formato invalido. Dica: digite apenas o numero da linha e depois enter.\n");
			printf("Digite enter para continuar...");
			erro=1;
			getchar();
		}
		else if(pseudolinha[0]>=49 && pseudolinha[0]<=48+n){
			for(i=49; i<=48+n; i++){
				if(pseudolinha[0]==i){                                                                       //validacao da linha
					linha=i-48;
					break;
				}
			}
		}
		else{
			printf("Formato invalido. Dica: digite apenas o numero da linha e depois enter.\n");
			printf("Digite enter para continuar...");
			erro=1;
			getchar();
		}
		if(erro==0){
			printf("Coluna:");
			fgets(pseudocoluna, 1000, stdin);
			int c=strlen(pseudocoluna);
			if(pseudolinha[c-1]=='\n'){
				pseudolinha[c-1]='\0';
			}
			if(c>2){
				printf("Formato invalido. Dica: digite apenas o numero da coluna e depois enter.\n");
				printf("Digite enter para continuar...");
				erro=1;
				getchar();
			}
			else if(pseudocoluna[0]>=49 && pseudocoluna[0]<=48+n){
				for(i=49; i<=48+n; i++){
					if(pseudocoluna[0]==i){                                                                 //validacao da coluna
						coluna=i-48;
						break;
					}
				}
			}
			else{
				printf("Formato invalido. Dica: digite apenas o numero da coluna e depois enter.\n");
				printf("Digite enter para continuar...");
				erro=1;
				getchar();
			}
		}
		if(erro==0){
			if(gabarito[linha-1][coluna-1]==1){
				printf("Ops! Numero errado! Digite enter para tentar novamente...");
				getchar();
				vidas--;
			}
			else if(matriz_usuario[linha-1][coluna-1]==0){
				printf("Voce ja apagou este numero, Mestre %s. Digite enter para prosseguir...", username);
				getchar();
			}
			else if(gabarito[linha-1][coluna-1]==0){
				matriz_usuario[linha-1][coluna-1]=0;
				for(i=0; i<n; i++){
					for(j=0; j<n; j++){
						gablinha[i]+=matriz_usuario[i][j];
					}
					if(gablinha[i]==somalinha[i]){
						printf("Parabens %s! Voce fechou a linha %d!\n", username, i+1);
						somalinha[i]=0;
					}
					gablinha[i]=0;
				}
				for(i=0; i<n; i++){
					for(j=0; j<n; j++){
						gabcoluna[i]+=matriz_usuario[j][i];
					}
					if(gabcoluna[i]==somacoluna[i]){
						printf("Parabens %s! Voce fechou a coluna %d!\n", username, i+1);
						somacoluna[i]=0;
					}
					gabcoluna[i]=0;
				}
				printf("Muito bem, Mestre! Digite enter para continuar...");
				getchar();
			}
		}
		flag=1;
		ganhou=0;
		for(i=0; i<n; i++){
			if(somacoluna[i]==0){ 
				ganhou++;
			}                               // a cada linha ou coluna fechada, a variavel ganhou eh incrementada em uma unidade
			if(somalinha[i]==0){
				ganhou++;
			}
		}
		if(vidas==0){
			system(CLEAR);
			printmatrizu();
			printf("Vish! Acabaram suas vidas! Digite enter para tentar de novo...");
			getchar();
		}
		if(ganhou==n*2){             // assim, quando a variavel ganhou atinge o valor de n*2, todas as linhas e colunas foram fechadas
			system(CLEAR);
			printmatrizu();     //mostra como ficou a matriz no fim do jogo
			rank();             //faz as alteracoes nessecarias no ranking
			nivel++;            //aumenta o nivel para pular para a proxima fase
			nivelcontrol();  //controla o nivel para mudar a dificuldade quando necessario
			progressocontrol();  //controla o progresso do usuario
			printf("Excelente mestre! Voce concluiu esta fase. Digite enter para irmos para a proxima...");
			getchar();
			checkzerou(); //verifica se o jogador zerou o jogo
		}
	}
	fclose(fd);
}

void menuprincipal();

void leoptionmainmenu(){ //funcao que le e valida a opcao do menu principal
	int i, n;
	fgets(pseudooptionmainmenu, 1000, stdin);
	n=strlen(pseudooptionmainmenu);
	if(pseudooptionmainmenu[n-1]=='\n'){
		pseudooptionmainmenu[n-1]='\0';
	}
	if(n>2){
		printf("Opcao invalida. Digite enter para tentar novamente...");
		getchar();
		menuprincipal();
	}
	else if(pseudooptionmainmenu[0]>=49 && pseudooptionmainmenu[0]<=53){
		for(i=49; i<=53; i++){
			if(pseudooptionmainmenu[0]==i){
				optionmainmenu=i-48;
				break;
			}
		}
	}
	else{
		printf("Opcao invalida. Digite enter para tentar novamente...");
		getchar();
		menuprincipal();
	}
}

void menuprincipal(){ // funcao que mostra o menu principal
	system(CLEAR);
	printf("Option 1: Jogar\n");
	printf("Option 2: Configuracoes\n");
	printf("Option 3: Ranking\n");
	printf("Option 4: Instrucoes\n");
	printf("Option 5: Sair\n");
	printf("O que vamos fazer, Mestre %s? ", username);
	leoptionmainmenu();
}

void zeraranking(){ //funcao que zera o ranking apenas abrindo o arquivo para escrita, que descarta o conteudo anterior
	fp=fopen("ranking.bin", "wb");
	qtdranking=0;
	fwrite(&qtdranking, sizeof(int), 1, fp);
	fclose(fd);
}

void menuconfig();

void leoptionconfigmenu(){ //funcao que le e valida a opcao do menu de configuracoes
	int i, n;
	fgets(pseudooptionconfigmenu, 1000, stdin);
	n=strlen(pseudooptionconfigmenu);
	if(pseudooptionconfigmenu[n-1]=='\n'){
		pseudooptionconfigmenu[n-1]='\0';
	}
	if(n>2){
		printf("Opcao invalida. Digite enter para tentar novamente...");
		getchar();
		menuconfig();
	}
	else if(pseudooptionconfigmenu[0]>=49 && pseudooptionconfigmenu[0]<=51){
		for(i=49; i<=53; i++){
			if(pseudooptionconfigmenu[0]==i){
				optionconfigmenu=i-48;
				break;
			}
		}
	}
	else{
		printf("Opcao invalida. Digite enter para tentar novamente...");
		getchar();
		menuconfig();
	}
}

void menudif();

void ledif(){ //funcao que le e valida a opcao do menu de dificuldades
	int i, n;
	fgets(pseudodif, 1000, stdin);
	n=strlen(pseudodif);
	if(pseudodif[n-1]=='\n'){
		pseudodif[n-1]='\0';
	}
	if(n>2){
		printf("Opcao invalida. Digite enter para tentar novamente...");
		getchar();
		menudif();
	}
	else if(pseudodif[0]>=49 && pseudodif[0]<=52){
		for(i=49; i<=52; i++){
			if(pseudodif[0]==i){
				dif=i-48;
				break;
			}
		}
	}
	else{
		printf("Opcao invalida. Digite enter para tentar novamente...");
		getchar();
		menudif();
	}
}

int validazera(){ //funcao que valida a opcao de zerar ranking
	int i;
	int n=strlen(pseudozera);
	if(pseudozera[n-1]=='\n'){
		pseudozera[n-1]='\0';
	}
	if(n>2){
		return 0;
	}
	if(pseudozera[0]=='n' || pseudozera[0]=='N' || pseudozera[0]=='s' || pseudozera[0]=='S'){
	return 1;
	}
	return 0;
}

void lezera(){ //funcao que le a opcao de zerar ranking
	fgets(pseudozera, 1000, stdin);
	if(!validazera()){
		printf("Opcao invalida. Digite enter para tentar novamente...");
		getchar();
		lezera();
	}
	else{
		zera=pseudozera[0];
	}
}

void loopgame();

void menudif(){ //funcao que mostra o menu de dificuldades
	system(CLEAR);
	printf("   ESCOLHA A DIFICULDADE   \n");
	printf("Option 1: Aprendiz\n");
	printf("Option 2: Veterano\n");
	printf("Option 3: Mestre\n");
	printf("Option 4: Voltar ao menu de configuracoes\n");
	printf("\n");
	printf("Escolha uma das opcoes, Mestre:", username);
	ledif();
}

void menuconfig(){ // funcao que mostra o menu de configuracoes
	system(CLEAR);
	printf("   CONFIGURACOES   \n");
	printf("\n");
	printf("Option 1: Selecionar dificuldade\n");
	printf("Option 2: Zerar ranking\n");
	printf("Option 3: Voltar ao menu principal\n");
	printf("\n");
	printf("O que vamos fazer, Mestre %s? ", username);
	leoptionconfigmenu();
	if(optionconfigmenu==1){
		menudif();
		if(dif==4){
			menuconfig();
		}
		else if(dif<1 || dif>4){
			printf("\nOpcao invalida. Digite enter para tentar de novo...");
			getchar();
		}
		else{
			dificuldade=dif;  //ajusta a dificuldade baseada na opcao escolhida
			if(dificuldade==1){
				nivel=0;
				printf("Dificuldade Aprendiz selecionada\n");
			}
			else if(dificuldade==2){
				nivel=4;
				printf("Dificuldade Veterano selecionada\n");
			}
			else if(dificuldade==3){
				nivel=8;
				printf("Dificuldade Mestre selecionada\n");
			}
			printf("Digite enter para continuar...");
			getchar();
		}
	}
	else if(optionconfigmenu==2){
		printf("Tem certeza de que deseja zerar o ranking, Mestre? (S caso sim, N caso nao) ");
		lezera();
		if(zera=='S' || zera=='s'){
			zeraranking();
			printf("Ranking zerado! Digite enter para continuar...");
			getchar();
			menuconfig();
		}
		else{
			printf("Zeramento do ranking cancelado! Digite enter para continuar...");
			getchar();
			menuconfig();
		}
	}
	else if(optionconfigmenu==3){
		loopgame();
	}
}

void menuinstrucoes(){ // funcao que mostra o menu de instrucoes
	system(CLEAR);
	printf("   INSTRUCOES   \n");
	printf("Ola, Mestre %s, boas vindas ao jogo Mestre das Somas!\n", username);
	printf("Ao abrir esse jogo, voce automaticamente se tornou meu mestre!\n");
	printf("Meu objetivo aqui eh te ajudar a passar por todas as fases do jogo para enfim, se tornar o verdadeiro Mestre das Somas!\n");
	printf("Dito isso, segue abaixo uma lista de instrucoes para guia-lo nessa jornada:\n");
	printf("-Para se tornar o Mestre das Somas, basta concluir todas as fases de todas as dificuldades(sao 3).\n");
	printf("-Cada fase consiste em um desafio que envolve somas de linhas e colunas de uma matriz, cujo tamanho depende da dificuldade escolhida.\n");
	printf("-Seu objetivo eh eliminar os numeros da matriz que nao pertencem a soma de nenhuma linha ou coluna, passando de fase ao eliminar todos corretamente.\n");
	printf("-Caso cometa um erro ao eliminar, uma vida sera perdida, mas nao se preocupe! Voce possui 5 vidas em cada fase.\n");
	printf("-Ao perder suas 5 vidas, voce perde a fase e tem de reinicia-la para continuar avancando.\n");
	printf("-Ao abrir o jogo, a dificuldade padrao eh a de Aprendiz, a mais facil de todas.\n");
	printf("-Apos terminar todas as fases de uma dificuldade, a dificuldade seguinte sera automaticamente selecionada.\n");
	printf("-A depender da dificuldade da fase concluida, sera atribuida a sua conta uma pontuacao, sendo:\n");
	printf("   >Aprendiz: 50 pontos.\n");
	printf("   >Veterano: 100 pontos.\n");
	printf("   >Mestre:   200 pontos.\n");
	printf("-Sua pontuacao atual sera salva no ranking, para poder comparar com outros Mestres!\n");
	printf("-Depois de concluir a ultima fase da dificuldade Mestre, voce tera finalmente zerado o jogo, se tornando o Mestre das Somas!\n");
	printf("Ufa! Agora, voce ja esta pronto para comecar sua jornada, vamos la?\n");
	printf("Digite enter para continuar...");
	getchar();
}

void menuzerou();

void leopzerou(){ //le e valida a opcao do menu zerou
	int i, n;
	fgets(pseudoopzerou, 1000, stdin);
	n=strlen(pseudoopzerou);
	if(pseudoopzerou[n-1]=='\n'){
		pseudoopzerou[n-1]='\0';
	}
	if(n>2){
		printf("Opcao invalida. Digite enter para tentar novamente...");
		getchar();
		menuzerou();
	}
	else if(pseudoopzerou[0]>=49 && pseudoopzerou[0]<=50){
		for(i=49; i<=50; i++){
			if(pseudoopzerou[0]==i){
				opzerou=i-48;
				break;
			}
		}
	}
	else{
		printf("Opcao invalida. Digite enter para tentar novamente...");
		getchar();
		menuzerou();
	}
}

void menuzerou(){ // funcao que mostra o menu que aparece ao zerar o jogo
	system(CLEAR);
	printf("    PARABENS MESTRE %s!!!    \n", username);
	printf("Finalmente, depois de uma longa jornada provando sua maestria nas somas, voce zerou o jogo Mestre das Somas!\n");
	printf("Agora que ja conhece todo o caminho, pode percorre-lo novamente ou finalizar por aqui...\n");
	printf("1-Continuar\n");
	printf("2-Finalizar\n");
	printf("O que deseja, Mestre? ");
	leopzerou();
	if(opzerou==2){
		fechar=1;
	}
	else{
		loopgame();
	}
}

void printatelainicial(){ //funcao que printa a tela inicial
	system(CLEAR);
	printf("Bem vindo(a) ao jogo Mestre das somas!\n");
	printf("Digite seu username, Mestre:");
}

int validausername(){ //funcao que valida o username
	int i;
	int n=strlen(pseudousername);
	if(pseudousername[n-1]=='\n'){
		pseudousername[n-1]='\0';
	}
	if(n>31){
		return 0;
	}
	for(i=0; i<n-1; i++){
		if(pseudousername[i]<=47){
			return 0;
		}
		else if(pseudousername[i]>=58 && pseudousername[i]<=64){
			return 0;
		}
		else if(pseudousername[i]>=91 && pseudousername[i]<=96){
			return 0;
		}
		else if(pseudousername[i]>=123){
			return 0;
		}
		else if(pseudousername[0]>=48 && pseudousername[0]<=57){
			return 0;
		}
	}
	return 1;
}

void leusername(){ //funcao que le o username
	fgets(pseudousername, 1000, stdin);
	if(!validausername()){
		printf("\nUsername invalido! Evite nomes maiores que 30 caracteres, que iniciam com numero ou que tenham caracteres especiais e/ou\nespacos.\n");
		printf("\nDigite enter para inserir outro username...");
		getchar();
		printatelainicial();
		leusername();
	}
	else{
		strcpy(username, pseudousername);
		printf("Ok, mestre %s, vamos la! Digite enter para continuar...", username);
		getchar();
	}
}

void loopgame(){ //loop principal do jogo
	int z=0; //variavel que sinaliza quando o jogador zerou o jogo, para que o menu zerou so apareca somente apos o jogador zerar
	do{
		if(zerou==1 && z==0){ //mostra o menu zerou caso o jogador tenha zerado e nunca tenha visto o menu 
			menuzerou();
			z=1;
			if(fechar==1){
				break;
			}
		}
		menuprincipal();
		if(optionmainmenu==1){
			jogar();
		}
		else if(optionmainmenu==2){
			menuconfig();
		}
		else if(optionmainmenu==3){
			menuranking();
		}
		else if(optionmainmenu==4){
			menuinstrucoes();
		}
	}while(optionmainmenu!=5);
}

int main(){ //funcao main
	printatelainicial();
	leusername();
	loopgame();
	return 0;
}
