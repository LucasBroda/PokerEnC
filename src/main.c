#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "resource_dir.h"	
#include "raylib.h"


// Définir la taille des cartes
#define CARD_WIDTH 60
#define CARD_HEIGHT 90

//  Structure pour les cartes du jeu 
struct Cartes{
    int face;
    int couleur;
};

//  Structure pour les mains des joueurs, permet notamment de définir que les joueurs auront une main de 5 cartes
struct Main{
    struct Cartes cartes[5];
};

//  Structure pour le jeu de cartes, permet de définir que le jeu de cartes 
// contiendra 52 cartes et de définir le nombre de cartes distribuées au total
struct Deck{
    struct Cartes cartesDistribuées[52];
    int nombresDeCartesDistribuées;
};

// Déclaration d'attributs représentant les mains des joueurs et le jeu de cartes
struct Main Main1, Main2;
struct Deck deck;

// Arrays qui contiennent les noms des faces, des couleurs et des rangs des mains
char *faceStrings[] = {
    "Deux","Trois","Quatre","Cinq","Six","Sept","Huit","Neuf",
    "Dix","Valet","Reine","Roi","As"};
char *couleurStrings[] = {
    "Coeur","Carreau","Pique","Trèfle"};
// Ici, je fais deux tableaux contenant le nom des symboles et des valeurs en majuscules, ou avec les valeurs numériques
// Car mes images sont nommées de la sorte (ex: 2COEUR.png, 3CARREAU.png, etc)
const char* couleurEnMajPourAfficher[] = {
	"COEUR", "CARREAU", "TREFLE", "PIQUE"
};
const char* faceStringsEnMajPourAfficher[] = {
	"2", "3", "4", "5", "6", "7", "8", "9", "10", "VALET", "REINE", "ROI", "AS"
};
char *rankStrings[] = {
    "des cartes avec de hautes valeurs","une Paire","deux Paires","trois Paires","Quinte",
    "Flush","Full House","Carré","Quinte Flush"};

// Permet de gérer la distribution des cartes aux deux joueurs
struct Cartes distributionCartes() {
    struct Cartes cartesADistribuer;
    // Variable permettant de vérifier si la carte est unique ou non
    bool estUnique;

    do {
        // Génération aléatoire de la face et de la couleur de la carte
        cartesADistribuer.face = rand() % 13;
        cartesADistribuer.couleur = rand() % 4;
        // On part du principe que la carte est unique au départ
        estUnique = true;

        // On vérifie si la carte est unique
        for (int i = 0; i < deck.nombresDeCartesDistribuées; i++) {
            if (deck.cartesDistribuées[i].face == cartesADistribuer.face && deck.cartesDistribuées[i].couleur == cartesADistribuer.couleur) {
                estUnique = false;
                break;
            }
        }
    } while (!estUnique);


    // Si la carte est unique, on l'ajoute au deck
    deck.cartesDistribuées[deck.nombresDeCartesDistribuées++] = cartesADistribuer;
    return cartesADistribuer;
}

// Permet de générer les mains des joueurs
void generationsMains(){
    //  Distribution de cinq cartes aux deux joueurs
    for(int i=0; i<5; i++){
        Main1.cartes[i] = distributionCartes();
        Main2.cartes[i] = distributionCartes();
    }
}

// Permet d'analyser les mains des joueurs pour déterminer le gagnant
void analyzeMains() {
    // Déclaration les variables nécessaires pour l'analyse des mains des joueurs
    int Main1CompteurFaces[13] = {0};
    int Main1CompteurCouleurs[4] = {0};
    int Main2CompteurFaces[13] = {0};
    int Main2CompteurCouleurs[4] = {0};

    // On compte le nombre de faces et de couleurs pour chaque main
    for (int i = 0; i < 5; i++) {
        Main1CompteurFaces[Main1.cartes[i].face]++;
        Main1CompteurCouleurs[Main1.cartes[i].couleur]++;
        Main2CompteurFaces[Main2.cartes[i].face]++;
        Main2CompteurCouleurs[Main2.cartes[i].couleur]++;
    }

    // Variables permettant de traquer les cartes des joueurs
    int h1Rank = 0, h1Consec = 0, h1Quinte = 0, h1Flush = 0, h1Carré = 0, h1Trois = 0, h1Paires = 0;
    int h2Rank = 0, h2Consec = 0, h2Quinte = 0, h2Flush = 0, h2Carré = 0, h2Trois = 0, h2Paires = 0;

    // Permet de vérifier si les joueurs ont un flush
    for (int i = 0; i < 4; i++) {
        if (Main1CompteurCouleurs[i] == 5) h1Flush = 1;
        if (Main2CompteurCouleurs[i] == 5) h2Flush = 1;
    }

    // Permet de compter les faces consécutives non nulles
    for (int i = 0; i < 13; i++) {
        if (Main1CompteurFaces[i]) h1Consec++;
        else h1Consec = 0;
        if (h1Consec == 5) h1Quinte = 1;

        if (Main2CompteurFaces[i]) h2Consec++;
        else h2Consec = 0;
        if (h2Consec == 5) h2Quinte = 1;
    }

    // Permet de vérifier si les joueurs ont des paires, des brelans, des carrés
    for (int i = 0; i < 13; i++) {
        if (Main1CompteurFaces[i] == 4) h1Carré = 1;
        if (Main1CompteurFaces[i] == 3) h1Trois = 1;
        if (Main1CompteurFaces[i] == 2) h1Paires++;

        if (Main2CompteurFaces[i] == 4) h2Carré = 1;
        if (Main2CompteurFaces[i] == 3) h2Trois = 1;
        if (Main2CompteurFaces[i] == 2) h2Paires++;
    }

    // Permet de déterminer le rang de la main du joueur 1
    if (h1Quinte && h1Flush) h1Rank = 8;
    else if (h1Carré) h1Rank = 7;
    else if (h1Trois && h1Paires == 1) h1Rank = 6;
    else if (h1Flush) h1Rank = 5;
    else if (h1Quinte) h1Rank = 4;
    else if (h1Trois) h1Rank = 3;
    else if (h1Paires == 2) h1Rank = 2;
    else if (h1Paires == 1) h1Rank = 1;
    else h1Rank = 0;

    // Rang de la main du joueur 2
    if (h2Quinte && h2Flush) h2Rank = 8;
    else if (h2Carré) h2Rank = 7;
    else if (h2Trois && h2Paires == 1) h2Rank = 6;
    else if (h2Flush) h2Rank = 5;
    else if (h2Quinte) h2Rank = 4;
    else if (h2Trois) h2Rank = 3;
    else if (h2Paires == 2) h2Rank = 2;
    else if (h2Paires == 1) h2Rank = 1;
    else h2Rank = 0;

    // Affichage des rangs des mains des joueurs
	DrawText(TextFormat("Le joueur 1 a : %s", rankStrings[h1Rank]), 50, 500, 20, BLACK);
	DrawText(TextFormat("Le joueur 2 a : %s", rankStrings[h2Rank]), 50, 550, 20, BLACK);

    // Détermine le gagnant en fonction des rangs des mains des joueurs
    if (h1Rank > h2Rank) {
		// Permet d'afficher le joueur gagnant dans la fenêtre
		DrawText("Le joueur 1 a gagné la partie !", 50, 450, 20, BLACK);
	}
    else if (h2Rank > h1Rank) {
		// Permet d'afficher le joueur gagnant dans la fenêtre
		DrawText("Le joueur 2 a gagné la partie !", 50, 450, 20, BLACK);
	}
    else {
		// Permet d'afficher le joueur gagnant dans la fenêtre
		DrawText("Egalité entre les deux joueurs !", 50, 450, 20, BLACK);
	}
}

// Fonction pour charger toutes les images des cartes
void LoadCardTextures(Texture2D cardTextures[13][4]) {
    char filePath[50];
    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 4; j++) {
            snprintf(filePath, sizeof(filePath), "resources/%s%s.png", faceStringsEnMajPourAfficher[i], couleurEnMajPourAfficher[j]);
            cardTextures[i][j] = LoadTexture(filePath);
        }
    }
}

// Fonction pour décharger toutes les images des cartes
void UnloadCardTextures(Texture2D cardTextures[13][4]) {
    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 4; j++) {
            UnloadTexture(cardTextures[i][j]);
        }
    }
}

// Fonction pour dessiner une carte
void DrawCard(struct Cartes carte, int posX, int posY, Texture2D cardTextures[13][4]) {
    DrawTexture(cardTextures[carte.face][carte.couleur], posX, posY, WHITE);
}

// Fonction pour dessiner la main d'un joueur
void DrawHand(struct Main main, int posX, int posY, const char* player, Texture2D cardTextures[13][4]) {
    DrawText(player, posX, posY - 20, 20, BLACK);
    for (int i = 0; i < 5; i++) {
        DrawCard(main.cartes[i], posX + i * (CARD_WIDTH + 10), posY, cardTextures);
    }
}

int main(){
    
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Poker Game");

	// Permet de réinitialiser le jeu des joueurs à chaque nouvelle partie
    srand(time(NULL));

	// Charger les images des cartes
    Texture2D cardTextures[13][4];
	LoadCardTextures(cardTextures);

	// Permet de charger un nouveau fond d'écran pour la fenêtre
	Texture2D background = LoadTexture("resources/9999520.jpg");
	// Permert de générer les mains des joueurs
	generationsMains();

    while (!WindowShouldClose()) {

        BeginDrawing();
        DrawTexture(background, 0, 0, WHITE);

        DrawHand(Main1, 50, 100, "Le joueur 1 a les cartes suivantes", cardTextures);
        DrawHand(Main2, 50, 300, "Le joueur 2 a les cartes suivantes", cardTextures);

		analyzeMains();

        EndDrawing();
    }

	UnloadTexture(background);
	UnloadCardTextures(cardTextures);
    CloseWindow();

    return 0;
}