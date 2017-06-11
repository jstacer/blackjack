/* 
This is a watered down version of Blackjack I wrote for school. One mode of play, no IOstream actions :)
Edited for Linux ec2 instance integration with my Rails app
*/


#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <array>
#include <random>
#include <chrono>
#include <cmath>

using namespace std;

// global structure
struct Player
{
	string username;
	double money;
	double bet1;
	double bet2;
	double bet3;
	double insurance;
	int numHands;
	vector<string> hand1;
	vector<string> hand2;
	vector<string> hand3;
};


// function prototypes
void playGame(int, fstream &);
void completeHands(int, Player &, Player &, vector<string> &, vector<string> &, fstream &);
void handTurn(int, Player &, Player &, vector<string> &, vector<string> &, fstream &, Player &, vector<string> &, int);
void askForInsurance(int, Player &, Player &, vector<string> &, vector<string> &, fstream &);
void dealerBlackjack(int, Player &, Player &, vector<string> &, vector<string> &, fstream &);
void displayHand(const vector<string> &, bool);
void displayAllHands(int, const Player &, const Player &, const vector<string> &, bool);
int firstChoiceSplit(int, Player &, Player &, vector<string> &, Player &, int);
int firstChoice(int, Player &, Player &, vector<string> &, Player &, int);
int choice(int, Player &, Player &, vector<string> &, Player &, int);
void payout(Player &, double &, fstream &, bool, bool = false);
void loss(Player &, double &, fstream &, bool);
void placeBet(Player &);
int getHandValue(vector<string> &);
int getCardValue(string);
void dealCard(vector<string> &, vector<string> &);


int main()
{
	int input;                                             // gets user input
	fstream accounts;   // for writing to accounts.dat, create if it doesn't exist
	//accounts.close();									   // closes until needed by a function
	string tempName;							           // stores name created by newAccount()

	system("clear");

	do{
		cout << "  ____   _                 _____  _  __     _           _____  _  __" << endl;
		cout << " |  _ \\ | |         /\\    / ____|| |/ /    | |   /\\    / ____|| |/ /" << endl;
		cout << " | |_) || |        /  \\  | |     | ' /     | |  /  \\  | |     | ' / " << endl;
		cout << " |  _ < | |       / /\\ \\ | |     |  <  _   | | / /\\ \\ | |     |  <  " << endl;
		cout << " | |_) || |____  / ____ \\| |____ | . \\| |__| |/ ____ \\| |____ | . \\ " << endl;
		cout << " |____/ |______|/_/    \\_\\\\_____||_|\\_\\\\____//_/    \\_\\\\_____||_|\\_\\ " << endl;
		cout << endl;
		cout << " ===================================================================" << endl;
		cout << endl;
		cout << "1. Play vs. CPU\n";
		cout << "2. Quit\n";
		cout << endl;
		cout << "Enter your selection (1-2): ";
		cin >> input;

		if(input < 1 || input > 2)
		{
			cout << endl << endl;
			cout << "You must enter either 1 or 2.\n";
			system("read");
		}else{
			if(input == 1){
				playGame(input, accounts);
			}
		} // end if-else

		system("clear");

	}while(input != 2);

	cout << endl << endl; 
	return 0;

} // end main()


/*
Name: playGame
Description: Main function for game logic, branches into three other
main branches: dealerBlackjack, askForInsurance and completeHands
Parameters:
int numPlayers: number of players
fstream &accounts: file stream for accounts.txt
Return: void
*/
void playGame(int numPlayers, fstream &accounts)
{
	system("clear");

	// array of all 52 cards in deck
	string cards[] = {"As", "2s", "3s", "4s", "5s", "6s", "7s", "8s", "9s", "10s", "Js", "Qs", "Ks",
	                  "Ac", "2c", "3c", "4c", "5c", "6c", "7c", "8c", "9c", "10c", "Jc", "Qc", "Kc",
					  "Ah", "2h", "3h", "4h", "5h", "6h", "7h", "8h", "9h", "10h", "Jh", "Qh", "Kh",
					  "Ad", "2d", "3d", "4d", "5d", "6d", "7d", "8d", "9d", "10d", "Jd", "Qd", "Kd"};

	vector<string> deck,	     // put cards into a vector for shuffling/dealing 
	               dealerHand;   // dealer's hand, [0] is hole card
	bool loggedIn;               // flags successful login
	string tempName;             // Temporary username storage used in login, newAccount and
								 //    to initialize Player structures
	char again;					 // determines whether another game will be played


	cout << "Enter your name: ";
	cin >> tempName;
	cout << endl;

	while(tempName.length() > 18)
	{
		cout << "Don't be an asshole. Shorter, please.\n";
		cout << "Re-enter username: ";
		cin >> tempName;
		cout << endl;
	}

	// make username uppercase
	transform(tempName.begin(), tempName.end(), tempName.begin(), ::toupper);

	//accounts.open("accounts.txt", ios::out|ios::app); 
	//accounts << tempName << "\n";
	//accounts.close();

	// LOG PLAYERS IN
	// Player 1
	// loggedIn = login(1, accounts, tempName, ""); 
	/*
	if(!loggedIn){
		cout << endl;
		newAccount(accounts, tempName);
	}
	*/
	// declare, initialize structure for Player 1
	Player p1;
	p1.username = tempName;
	p1.money = 500;
	p1.insurance = 0;

	
	//Player 2
	Player p2;   // must be declared regardless of value of numPlayers
	/*
	if(numPlayers==2){
		system("clear");
		loggedIn = login(2, accounts, tempName, p1.username);
		if(!loggedIn){
			cout << endl;
			newAccount(accounts, tempName);
		}
		// initialize p2
		p2.username = tempName;
		p2.money = 500;
		p2.insurance = 0;
	} // end if
	*/

	/* END PLAYER INITIALIZATION */


	do{
		system("clear");

		// clear all hands and deck
		deck.clear();
		dealerHand.clear();
		p1.hand1.clear();
		p1.hand2.clear();
		p1.hand3.clear();
		p2.hand1.clear();
		p2.hand2.clear();
		p2.hand3.clear();

		// reset bets
		p1.bet1 = 0;
		p1.bet2 = 0;
		p1.bet3 = 0;
		p1.insurance = 0;
		if(numPlayers==2){
			p2.bet1 = 0;
			p2.bet2 = 0;
			p2.bet3 = 0;
			p2.insurance = 0;
		}

		// reset numHands for each player
		p1.numHands = 1;
		if(numPlayers==2)
			p2.numHands = 1;

		// reinitialize deck
		vector<string> deck(cards, end(cards));  // put cards into a vector for shuffling/dealing 
		
		// shuffle deck
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		auto gen = std::default_random_engine(seed);
 		shuffle (deck.begin(), deck.end(), gen);

		// place bets
		placeBet(p1);
		if(numPlayers==2){
			cout << endl;
			placeBet(p2);
		}

		system("clear");

		cout << "Dealing cards ...\n";
		system("read");

		// DEAL to each player then dealer two times each
		for(int i=0; i<2; i++){
			dealCard(deck, p1.hand1);
			if(numPlayers == 2)
				dealCard(deck, p2.hand1);
			dealCard(deck, dealerHand);
		}

		system("clear");

		// INITIAL DISPLAY
		displayAllHands(numPlayers, p1, p2, dealerHand, true);  // true hides hole card

		// THREE MAIN BRANCHES
		if(getCardValue(dealerHand[0]) == 1 && getCardValue(dealerHand[1]) == 10){
			dealerBlackjack(numPlayers, p1, p2, deck, dealerHand, accounts);
		}else if(getCardValue(dealerHand[1]) == 1){
			askForInsurance(numPlayers, p1, p2, deck, dealerHand, accounts);
		}else{
			completeHands(numPlayers, p1, p2, deck, dealerHand, accounts);
		} // end if

		// CHECK PLAYERS' MONEY, GET AGAIN CHOICE
		cout << endl;
		if(p1.money <= 0){     // force exit if one of players is out of money
			system("clear");
			cout << p1.username << " is out of money. Start a new game to buy more chips.\n\n";
			//system("read");
			again = 'n';
		}else if(numPlayers == 2 && p2.money <= 0){
			system("clear");
			cout << p2.username << " is out of money. Start a new game to buy more chips.\n\n";
			//system("read");
			again = 'n';
		}else{
			cout << "Do you want to play again? Enter Y for yes, or any other character\n\n" 
				 << "to return to the main menu: ";
			cin >> again;
		} // end if


		// Tell user what they won/lost, Bob
		if ((again != 'y') && (again != 'Y')) {
			double amt = p1.money - 500;
			if (amt > 0){
				cout << "You won $" << amt << "!";
			}else if (amt < 0){
				cout << "You lost $" << abs(amt);
			}else{
				cout << "You broke even";
			}
			cout << "\n";
			system("read");
		}

	}while(again == 'y' || again == 'Y');

} // end playGame()



/*
Name: completeHands
Description: Game logic for completing hands if there's no dealer blackjack
Parameters:
numPlayers: number of players
Player &p1: player 1 structure
Player &p2: player 2 structure
vector<string> &deck: Deck 
vector<string> &dealerHand: dealer's hand
fstream &accounts: file stream to accounts.txt
Return: void
*/
void completeHands(int numPlayers, Player &p1, Player &p2, vector<string> &deck, 
				   vector<string> &dealerHand, fstream &accounts)
{
	int choice;   // contains choice made

	system("clear");

	displayAllHands(numPlayers, p1, p2, dealerHand, true);

	// PLAYER
	// ONE
	cout << p1.username << "'s turn.\n";
	handTurn(numPlayers, p1, p2, dealerHand, deck, accounts, p1, p1.hand1, 1);

	system("clear");
	displayAllHands(numPlayers, p1, p2, dealerHand, true);

	if(p1.numHands > 1){   //P1 HAND2
		cout << p1.username << ", make a choice for your second hand.\n";
		handTurn(numPlayers, p1, p2, dealerHand, deck, accounts, p1, p1.hand2, 2);
		system("clear");
		displayAllHands(numPlayers, p1, p2, dealerHand, true);
	}	

	if(p1.numHands > 2){   //P1 HAND3
		cout << p1.username << ", make a choice for your third hand.\n";
		handTurn(numPlayers, p1, p2, dealerHand, deck, accounts, p1, p1.hand3, 3);
		system("clear");
		displayAllHands(numPlayers, p1, p2, dealerHand, true);
	}

	// PLAYER
	// TWO
	if(numPlayers == 2){
		cout << p2.username << "'s turn.\n";
		handTurn(numPlayers, p1, p2, dealerHand, deck, accounts, p2, p2.hand1, 1);
	
		system("clear");
		displayAllHands(numPlayers, p1, p2, dealerHand, true);

		if(p2.numHands > 1){   //P2 HAND2
			cout << p2.username << ", make a choice for your second hand.\n";
			handTurn(numPlayers, p1, p2, dealerHand, deck, accounts, p2, p2.hand2, 2);
			system("clear");
			displayAllHands(numPlayers, p1, p2, dealerHand, true);
		}	

		if(p2.numHands > 2){   //P1 HAND3
			cout << p2.username << ", make a choice for your third hand.\n";
			handTurn(numPlayers, p1, p2, dealerHand, deck, accounts, p2, p2.hand3, 3);
			system("clear");
			displayAllHands(numPlayers, p1, p2, dealerHand, true);
		}
	} // end if 2 Players
	
	system("clear");
	displayAllHands(numPlayers, p1, p2, dealerHand, true);
	cout << "Dealer completes hand.\n";
	system("read");

	// DEALER FINISH HAND, reveal hole card
	system("clear");
	displayAllHands(numPlayers, p1, p2, dealerHand, false);

	while(getHandValue(dealerHand) < 17){
		dealCard(deck, dealerHand);
		system("clear");	
		displayAllHands(numPlayers, p1, p2, dealerHand, false);
	}

	if(getHandValue(dealerHand) > 21){ // DEALER BUST
		dealerHand[0] = "BUST";
		system("clear");
		displayAllHands(numPlayers, p1, p2, dealerHand, false);
		system("read");
	} // end if

	//cout << "\n***Dealer has " << getHandValue(dealerHand) << "***";

	//********************************************************************************
	// PAYOUTS, take busts and surrenders into account (money already taken)
	// test for [0] "BUST" or "SURRENDER" and also number of cards in hand for Players
	//********************************************************************************
	
	//PLAYER 1
	  //HAND 1
	if(p1.hand1[0] != "BUST" && p1.hand1[0] != "SURRENDER")
	{
		if(getHandValue(p1.hand1) > getHandValue(dealerHand) || dealerHand[0] == "BUST")  // win
		{
			if(p1.hand1.size() == 2 && getHandValue(p1.hand1) == 21){    //blackjack                   			
				payout(p1, p1.bet1, accounts, true);  
				p1.hand1[0] = "BLACKJACK";
				system("clear");
				displayAllHands(numPlayers, p1, p2, dealerHand, false);
				cout << p1.username << "'s first hand is a Blackjack!\n";
				system("read");
			}else{                                                       //non-bj win
				payout(p1, p1.bet1, accounts, false); 
				p1.hand1[0] = "WIN";
				system("clear");
				displayAllHands(numPlayers, p1, p2, dealerHand, false);
				cout << p1.username << " wins first hand!\n";
				system("read");
			}
		}else{                                                            // loss
				loss(p1, p1.bet1, accounts, false);
				system("clear");
				displayAllHands(numPlayers, p1, p2, dealerHand, false);
				cout << p1.username << " loses first hand.\n";
				system("read");
		}
	} // end if not bust or surrender by player

	//PLAYER 1
	  //HAND 2
	if(p1.numHands > 1 && p1.hand2[0] != "BUST" && p1.hand2[0] != "SURRENDER")
	{
		if(getHandValue(p1.hand2) > getHandValue(dealerHand) || dealerHand[0] == "BUST")
		{
			if(p1.hand2.size() == 2 && getHandValue(p1.hand2) == 21){                         			
				payout(p1, p1.bet2, accounts, true);  //blackjack
				p1.hand2[0] = "BLACKJACK";
				system("clear");
				displayAllHands(numPlayers, p1, p2, dealerHand, false);
				cout << p1.username << "'s second hand is a Blackjack!\n";
				system("read");
			}else{
				payout(p1, p1.bet2, accounts, false); //non-bj win
				p1.hand2[0] = "WIN";
				system("clear");
				displayAllHands(numPlayers, p1, p2, dealerHand, false);
				cout << p1.username << " wins second hand!\n";
				system("read");
			}
		}else{
				loss(p1, p1.bet2, accounts, false);
				system("clear");
				displayAllHands(numPlayers, p1, p2, dealerHand, false);
				cout << p1.username << " loses second hand.\n";
				system("read");
		}
	} // end if not bust or surrender by player
	
	//PLAYER 1
	  //HAND 3
	if(p1.numHands > 2 && p1.hand3[0] != "BUST" && p1.hand3[0] != "SURRENDER")
	{
		if(getHandValue(p1.hand3) > getHandValue(dealerHand) || dealerHand[0] == "BUST")
		{
			if(p1.hand3.size() == 2 && getHandValue(p1.hand3) == 21){                         			
				payout(p1, p1.bet3, accounts, true);  //blackjack
				p1.hand3[0] = "BLACKJACK";
				system("clear");
				displayAllHands(numPlayers, p1, p2, dealerHand, false);
				cout << p1.username << "'s third hand is a Blackjack!\n";
				system("read");
			}else{
				payout(p1, p1.bet3, accounts, false); //non-bj win
				p1.hand3[0] = "WIN";
				system("clear");
				displayAllHands(numPlayers, p1, p2, dealerHand, false);
				cout << p1.username << " wins third hand!\n";
				system("read");
			}
		}else{
				loss(p1, p1.bet3, accounts, false);
				system("clear");
				displayAllHands(numPlayers, p1, p2, dealerHand, false);
				cout << p1.username << " loses third hand.\n";
				system("read");
		}
	} // end if not bust or surrender by player


	//PLAYER 2
	if(numPlayers == 2){
		//HAND 1
		if(p2.hand1[0] != "BUST" && p2.hand1[0] != "SURRENDER")
		{
			if(getHandValue(p2.hand1) > getHandValue(dealerHand) || dealerHand[0] == "BUST")
			{
				if(p2.hand1.size() == 2 && getHandValue(p2.hand1) == 21){                         			
					payout(p2, p2.bet1, accounts, true);  //blackjack
					p2.hand1[0] = "BLACKJACK";
					system("clear");
					displayAllHands(numPlayers, p1, p2, dealerHand, false);
					cout << p2.username << "'s first hand is a Blackjack!\n";
					system("read");
				}else{
					payout(p2, p2.bet1, accounts, false); //non-bj win
					p2.hand1[0] = "WIN";
					system("clear");
					displayAllHands(numPlayers, p1, p2, dealerHand, false);
					cout << p2.username << " wins first hand!\n";
					system("read");
				}
			}else{
					loss(p2, p2.bet1, accounts, false);
					system("clear");
					displayAllHands(numPlayers, p1, p2, dealerHand, false);
					cout << p2.username << " loses first hand.\n";
					system("read");
			}
		} // end if not bust or surrender by player

		//PLAYER 2
	 	  //HAND 2
		if(p2.numHands > 1 && p2.hand2[0] != "BUST" && p2.hand2[0] != "SURRENDER")
		{
			if(getHandValue(p2.hand2) > getHandValue(dealerHand) || dealerHand[0] == "BUST")
			{
				if(p2.hand2.size() == 2 && getHandValue(p2.hand2) == 21){                         			
					payout(p2, p2.bet2, accounts, true);  //blackjack
					p2.hand2[0] = "BLACKJACK";
					system("clear");
					displayAllHands(numPlayers, p1, p2, dealerHand, false);
					cout << p2.username << "'s second hand is a Blackjack!\n";
					system("read");
				}else{
					payout(p2, p2.bet2, accounts, false); //non-bj win
					p2.hand2[0] = "WIN";
					system("clear");
					displayAllHands(numPlayers, p1, p2, dealerHand, false);
					cout << p2.username << " wins second hand!\n";
					system("read");
				}
			}else{
					loss(p2, p2.bet2, accounts, false);
					system("clear");
					displayAllHands(numPlayers, p1, p2, dealerHand, false);
					cout << p2.username << " loses second hand.\n";
					system("read");
			}
		} // end if not bust or surrender by player

		//PLAYER 2
		  //HAND 3
		if(p2.numHands > 2 && p2.hand3[0] != "BUST" && p2.hand3[0] != "SURRENDER")
		{
			if(getHandValue(p2.hand3) > getHandValue(dealerHand) || dealerHand[0] == "BUST")
			{
				if(p2.hand3.size() == 2 && getHandValue(p2.hand3) == 21){                         			
					payout(p2, p2.bet3, accounts, true);  //blackjack
					p2.hand3[0] = "BLACKJACK";
					system("clear");
					displayAllHands(numPlayers, p1, p2, dealerHand, false);
					cout << p2.username << "'s third hand is a Blackjack!\n";
					system("read");
				}else{
					payout(p2, p2.bet3, accounts, false); //non-bj win	
					p2.hand3[0] = "WIN";
					system("clear");
					displayAllHands(numPlayers, p1, p2, dealerHand, false);
					cout << p2.username << " wins third hand!\n";
					system("read");
				}
			}else{
					loss(p2, p2.bet3, accounts, false);
					system("clear");
					displayAllHands(numPlayers, p1, p2, dealerHand, false);
					cout << p2.username << " loses third hand.\n";
					system("read");
			}
		} // end if not bust or surrender by player
	} // end if numplayers==2
} // end completeHands()



/*
Name: handTurn
Description: Game logic for completing a hand for a player
Parameters:
numPlayers: number of players
Player &p1: player 1 structure
Player &p2: player 2 structure
vector<string> &deck: Deck 
vector<string> &dealerHand: dealer's hand
fstream &accounts: file stream to accounts.txt
Player &p: contextual player structure (whose hand is being completed)
vector<string> &hand: hand being completed
int currHand: which hand of a player is being completed
Return: void
*/
void handTurn(int numPlayers, Player &p1, Player &p2, vector<string> &dealerHand, vector<string> &deck, 
			  fstream &accounts, Player &p, vector<string> &hand, int currHand)
{
	int playerChoice;      // stores choice made
	string card;           // store card temporarily
	double winnings;       // for calculating bust losses on accounts.dat
	double lossAmt;        // used for calculating new winnings
	bool splitAce = false; // flags a hand containing a split ace

	// this loop performs SPLITS before other hand actions
	do{
		if(getCardValue(hand[0]) == getCardValue(hand[1]))
		{
			if(p.numHands < 3)				
				playerChoice = firstChoiceSplit(numPlayers, p1, p2, dealerHand, p, currHand);	
			else
				playerChoice = firstChoice(numPlayers, p1, p2, dealerHand, p, currHand);
		}else
			playerChoice = firstChoice(numPlayers, p1, p2, dealerHand, p, currHand);
	
		if(playerChoice == 5){   // PERFORM A SPLIT
			if(p.numHands == 1){	   // first split
				//perform split by taking card from one hand, placing in next
				if(getCardValue(hand[0]) == 1)
					splitAce = true;
				p.bet2 = p.bet1;
				p.money = p.money-p.bet1;
				card = hand[hand.size() - 1];
				hand.pop_back();
				p.hand2.push_back(card);
				dealCard(deck, hand);
				dealCard(deck, p.hand2);
			} // end if

			if(p.numHands == 2){       // second split	
				p.bet3 = p.bet1;
				p.money = p.money-p.bet1;
				card = hand[hand.size() - 1];
				hand.pop_back();
				p.hand3.push_back(card);
				dealCard(deck, hand);
				dealCard(deck, p.hand3);				
			} // end if

			p.numHands++; // increment numHands
			system("clear");
			displayAllHands(numPlayers, p1, p2, dealerHand, true);
			cout << "***Hand Split***\n";

		} // end if choice == 5

		if(currHand > 1 && getCardValue(hand[0]) == 1)
			splitAce = true;

	}while(playerChoice == 5);  // end split loop


	if(playerChoice == 1){ // *******HITS********
		if(getHandValue(hand) == 21)
			cout << "You have Blackjack! You should stand.\n";
		if(getHandValue(hand) < 21){
			do{
				dealCard(deck, hand);
				system("clear");				
				displayAllHands(numPlayers, p1, p2, dealerHand, true);
				if(getHandValue(hand) < 21)
					playerChoice = choice(numPlayers, p1, p2, dealerHand, p, currHand);
			}while(playerChoice == 1 && getHandValue(hand) < 21);		
		} // end if
		if(getHandValue(hand) == 21 && hand.size() > 2){
			cout << "You have 21.\n";
			system("read");
			playerChoice = 2;
		}if(getHandValue(hand) > 21){
			cout << "You busted.\n";
			hand[0] = "BUST"; 
			system("read");
				
			if(currHand == 1) {     // take away bet
				loss(p, p.bet1, accounts, false);
			}else if(currHand == 2){
				loss(p, p.bet2, accounts, false);
			}else if(currHand == 3){
				loss(p, p.bet3, accounts, false);
			}
		} // end if
	} // end HITS if

	//STAND requires no action

	//DOUBLE DOWN
	if(playerChoice == 3){
		cout << p.username << ", your bet will double and you will receive only one more card.\n";
		system("read");
		if(currHand == 1){
			p.money -= p.bet1;
			p.bet1 = p.bet1 * 2;
		}else if(currHand == 2){
			p.money -= p.bet2;
			p.bet2 = p.bet2 * 2;
		}else if(currHand == 3){
			p.money -= p.bet3;
			p.bet3 = p.bet3 * 2;
		}
		dealCard(deck, hand);
		system("clear");
		displayAllHands(numPlayers, p1, p2, dealerHand, true);

		if(getHandValue(hand) > 21){  //BUST
			cout << p.username << ", you busted.\n";
			hand[0] = "BUST"; 
			system("read");
				
			if(currHand == 1) {     // take away bet
				loss(p, p.bet1, accounts, false);
			}else if(currHand == 2){
				loss(p, p.bet2, accounts, false);
			}else if(currHand == 3){
				loss(p, p.bet3, accounts, false);
			}
		} // end if BUST
	} // end if DOUBLE DOWN

	// offer SURRENDER option if only 2 cards
	if(playerChoice == 4 && hand.size() == 2){
		cout << p.username << ", you lose half your bet and forfeit this hand.\n";
		system("read");

		hand[0] = "SURRENDER";
		
		if(currHand == 1) {     // take away half bet
			loss(p, p.bet1, accounts, true);
		}else if(currHand == 2){
			loss(p, p.bet2, accounts, true);
		}else if(currHand == 3){
			loss(p, p.bet3, accounts, true);
		}
	} // end if SURRENDER
} // end handTurn()



/*
Name: askForInsurance
Description: Game logic for when dealer's up card is an Ace
Parameters:
numPlayers: number of players
Player &p1: player 1 structure
Player &p2: player 2 structure
vector<string> &deck: Deck 
vector<string> &dealerHand: dealer's hand
fstream &accounts: file stream to accounts.txt
Return: void
*/
void askForInsurance(int numPlayers, Player &p1, Player &p2, vector<string> &deck, 
					 vector<string> &dealerHand, fstream &accounts)
{
	char answer;	  // stores yes or no answer
	double winnings;  // temp storage for winnings calculation

	cout << "***DEALER ACE***\n\n";

	cout << p1.username << ", do you want insurance? (enter Y/N): ";
	cin >> answer;
	while(answer != 'y' && answer != 'Y' && answer != 'n' && answer != 'N'){
		cout << "\nYou must enter either Y or N. Re-enter answer: ";
		cin >> answer;
	}
	if(answer == 'y' || answer == 'Y'){
		if(p1.money > 0){
			cout << "\nEnter amount of insurance. You can bet up to half\n"
				 << "of your original bet: ";
			cin >> p1.insurance;
			while(p1.insurance > p1.money || p1.insurance <= 0 || p1.insurance > (p1.bet1 / 2))
			{
				cout << "\nInsurance bet must be greater than zero, cannot be more\n" 
			         <<	"than your money total or more than half your original bet.\n"
				 	 << "Re-enter insurance: ";
				cin >> p1.insurance;
			}
			p1.money = p1.money - p1.insurance;  // take insurance out of money pot
		}else{
			cout << "\nMoney must be greater than zero to bet insurance.\n";
			system("read");
		} // end if money > 0
	} // end if (yes answer)

	system("clear");
	displayAllHands(numPlayers, p1, p2, dealerHand, true);
	cout << "***DEALER ACE***\n\n";

	if(numPlayers == 2){
		cout << p2.username << ", do you want insurance? (enter Y/N): ";
		cin >> answer;
		while(answer != 'y' && answer != 'Y' && answer != 'n' && answer != 'N') 
		{
			cout << "\nYou must enter either Y or N. Re-enter answer: ";
			cin >> answer;
		}
		if(answer == 'y' || answer == 'Y'){
			if(p2.money > 0){
				cout << "\nEnter amount of insurance. You can bet up to half\n"
				     << "of your original bet: ";
				cin >> p2.insurance;
				while(p2.insurance > p2.money || p2.insurance <= 0 || p2.insurance > (p2.bet1 / 2))
				{
					cout << "\nInsurance bet must be greater than zero, cannot be more\n" 
			        	 << "than your money total or more than half your original bet.\n"
						 << "Re-enter insurance: ";
					cin >> p2.insurance;
				}
				p2.money = p2.money - p2.insurance;  // take insurance out of money pot
			}else{
				cout << "\nMoney must be greater than zero to bet insurance.\n";
				system("read");
			} // end if money > 0
		} // end if (yes answer)
	} // end if (numPlayers)

	// DEALER HAS BLACKJACK
	if(getCardValue(dealerHand[0]) == 10)
	{                  
		system("clear");
		displayAllHands(numPlayers, p1, p2, dealerHand, false);  // show hole card
		cout << "***DEALER BLACKJACK***\n\n";

		if(getHandValue(p1.hand1) < 21){    // loss
			cout << p1.username << " loses bet.\n";
			loss(p1, p1.bet1, accounts, false);
		}else{                              // push
			cout << p1.username << " has blackjack, keeps bet.\n";
			p1.money += p1.bet1;
			p1.bet1 = 0;
		} // end if
		
		if(p1.insurance > 0){               // insurance payout
			cout << p1.username << " wins 2x insurance.\n";
			payout(p1, p1.insurance, accounts, false, true);
		}

		// Player 2
		cout << endl;
		if(numPlayers==2){   
			if(getHandValue(p2.hand1) < 21){        // loss
				cout << p2.username << " loses bet.\n";
				loss(p2, p2.bet1, accounts, false);
			}else{                                  // push
				cout << p2.username << " has blackjack, keeps bet.\n";
				p2.money += p2.bet1;
				p2.bet1 = 0;
			} // end if-else

			if(p2.insurance > 0){                   // insurance payout
				cout << p2.username << " wins 2x insurance.\n";
				payout(p2, p2.insurance, accounts, false, true);
			}
		} // end if
		
	}else{     // NO DEALER BLACKJACK

		system("clear");
		displayAllHands(numPlayers, p1, p2, dealerHand, false);
		
		cout << "***Dealer does not have Blackjack***\n";
		cout << "\nPlayers lose any insurance bets placed.\n";

		// players lose their insurance
		loss(p1, p1.insurance, accounts, false);
		if(numPlayers == 2)
			loss(p2, p2.insurance, accounts, false);

		 // PLAY CONTINUES
		completeHands(numPlayers, p1, p2, deck, dealerHand, accounts); 
	} //end if-else
	
} // end askForInsurance()



/*
Name: dealerBlackjack
Description: Game logic in the event of dealer blackjack, with non-ace
as the up card
Parameters:
int numPlayers: number of players
Player &p1: Player 1 structure
Player &p2: Player 2 structure
vector<string> &deck: the deck
vector<string> &dealerHand: the dealer's hand
fstream &accounts: file stream to accounts.txt
Return: void
*/
void dealerBlackjack(int numPlayers, Player &p1, Player &p2, vector<string> &deck, 
			         vector<string> &dealerHand, fstream &accounts)
{
	system("clear");

	displayAllHands(numPlayers, p1, p2, dealerHand, false);

	cout << "***DEALER BLACKJACK***";

	if(getHandValue(p1.hand1) < 21){
		cout << "\n\n" << p1.username << " loses bet.";
		loss(p1, p1.bet1, accounts, false);
	}else{
		cout << "\n\n" << p1.username << " has blackjack, keeps bet.";
		p1.money += p1.bet1;
	} // end if

	if(numPlayers==2){
		if(getHandValue(p2.hand1) < 21){
			cout << "\n\n" << p2.username << " loses bet.";
			loss(p2, p2.bet1, accounts, false);
		}else{
			cout << "\n\n" << p2.username << " has blackjack, keeps bet.";
			p2.money += p2.bet1;
		} // end if

	} // end if

	cout << endl << endl;
	system("read");

} // end dealerBlackjack()



/*
Name: displayHand
Description: Displays cards in a hand or "bust", "surrender", "win" 
or "blackjack" where appropriate
Parameters:
const vector<string> &hand: Vector containing a hand
bool hideHole: Boolean that determines whether to hide hole card
Return: void
*/
void displayHand(const vector<string> &hand, bool hideHole)
{
	string card;

	if(hand[0] == "WIN") 
	{  
		for(int row = 0; row < 6; row++) 
		{
			if (row == 0)
				cout << " __          _______ _   _  \n";
			if (row == 1)
				cout << " \\ \\        / /_   _| \\ | | \n";
			if (row == 2)
				cout << "  \\ \\  /\\  / /  | | |  \\| | \n";
			if (row == 3)
				cout << "   \\ \\/  \\/ /   | | | . ` | \n";
			if (row == 4)
				cout << "    \\  /\\  /   _| |_| |\\  | \n";
			if (row == 5)
				cout << "     \\/  \\/   |_____|_| \\_| \n";
		} // end for

		return;  // exit function
	} // end win if

	if(hand[0] == "BLACKJACK") 
	{  
		for(int row = 0; row < 6; row++)
		{
			if (row == 0)
				cout << " ____  _               _____ _  __    _         _____ _  ___  \n";
			if (row == 1)
				cout << "|  _ \\| |        /\\   / ____| |/ /   | |  /\\   / ____| |/ / | \n";	
			if (row == 2)
				cout << "| |_) | |       /  \\ | |    | ' /    | | /  \\ | |    | ' /| | \n";
			if (row == 3)
				cout << "|  _ <| |      / /\\ \\| |    |  < _   | |/ /\\ \\| |    |  < | | \n";
			if (row == 4)
				cout << "| |_) | |____ / ____ \\ |____| . \\ |__| / ____ \\ |____| . \\|_| \n";
			if (row == 5)
				cout << "|____/|______/_/    \\_\\_____|_|\\_\\____/_/    \\_\\_____|_|\\_(_) \n";
		} //end for

		return;  //exit function
	} // end blackjack if

	if(hand[0] == "SURRENDER") 
	{ 
		for(int row = 0; row < 6; row++)
		{
			if(row==0)
				cout << "  _____ _    _ _____  _____  ______ _   _ _____  ______ _____   \n";
			if(row==1)
				cout << " / ____| |  | |  __ \\|  __ \\|  ____| \\ | |  __ \\|  ____|  __ \\  \n";
			if(row==2)
				cout << "| (___ | |  | | |__) | |__) | |__  |  \\| | |  | | |__  | |__) | \n";
			if(row==3)
				cout << " \\___ \\| |  | |  _  /|  _  /|  __| | . ` | |  | |  __| |  _  /  \n";
			if(row==4)
				cout << " ____) | |__| | | \\ \\| | \\ \\| |____| |\\  | |__| | |____| | \\ \\  \n";
			if(row==5)
				cout << "|_____/ \\____/|_|  \\_\\_|  \\_\\______|_| \\_|_____/|______|_|  \\_\\  \n";
		} //end for

		return; // exit function
	} // end if surrender

	if(hand[0] == "BUST") 
	{    
		for(int row = 0; row < 6; row++)
		{
			if(row==0)
				cout << " ____  _    _  _____ _______  \n";
			if(row==1)
				cout << "|  _ \\| |  | |/ ____|__   __| \n";
			if(row==2)
				cout << "| |_) | |  | | (___    | |    \n";
			if(row==3)
				cout << "|  _ <| |  | |\\___ \\   | |   \n";
			if(row==4)
				cout << "| |_) | |__| |____) |  | |   \n";
			if(row==5)
				cout << "|____/ \\____/|_____/   |_|  \n";
		} //end for
	
		return;    // exit funciton
	} // end if bust


	for(int row = 0; row < 6; row++)  // row loop
	{
		if(row==0)
			cout << ".";
		else if(row==5)
			cout << "'";
		else
			cout << "|";

		for(int c = 0; c < hand.size(); c++) // card loop
		{
			card = hand[c];   // current card in hand

			if(row==0)
				cout << "------.";
			else if(row==1)
			{			
				if(c==0 && hideHole)
				{
					cout << "      |";
					continue;
				}
				cout << card[0];
				if(card[0] == '1' && card[2] == 'h')        // extra logic for printing 10's 
					cout << "0  _ |";
				else if(card[0] == '1' && card[2] == 'd')
					cout << "0/\\  |";
				else if(card[0] == '1' && card[2] == 'c')
					cout << "0    |";
				else if(card[0] == '1' && card[2] == 's')
					cout << "0.   |";
				else if(card[0] != '1' && card[1]=='h')
					cout << "_  _ |";
				else if(card[0] != '1' && card[1]=='d')
					cout << " /\\  |";
				else if(card[0] != '1' && card[1]=='c')
					cout << " _   |";
				else if(card[0] != '1' && card[1]=='s')
					cout << " .   |";

			}else if(row==2){
				if(c==0 && hideHole)
				{
					cout << " hole |";
					continue;
				}
				if(card[1]=='h' || card[2]=='h')
					cout << "( \\/ )|";
				else if(card[1]=='d' || card[2]=='d')
					cout << " /  \\ |";
				else if(card[1]=='c' || card[2]=='c')
					cout << " ( )  |";
				else if(card[1]=='s' || card[2]=='s')
					cout << " / \\  |";
			}else if(row==3)
			{
				if(c==0 && hideHole)
				{
					cout << " card |";
					continue;
				}
				if(card[1]=='h' || card[2]=='h')
					cout << " \\  / |";
				else if(card[1]=='d' || card[2]=='d')
					cout << " \\  / |";
				else if(card[1]=='c' || card[2]=='c')
					cout << "(_x_) |";
				else if(card[1]=='s' || card[2]=='s')
					cout << "(_,_) |";
			}else if(row==4)
			{
				if(c==0 && hideHole)
				{
					cout << "      |";
					continue;
				}
				if(card[0] == '1' && card[2] == 'h')       // extra logic for printing 10's
					cout << "  \\/10|";
				else if(card[0] == '1' && card[2] == 'd')
					cout << "  \\/10|";
				else if(card[0] == '1' && card[2] == 'c')
					cout << "  Y 10|";
				else if(card[0] == '1' && card[2] == 's')
					cout << "  I 10|";
				else if(card[0] != '1' && card[1]=='h'){
					cout << "  \\/ ";
					cout << card[0];
					cout << "|";
				}else if(card[0] != '1' && card[1]=='d'){
					cout << "  \\/ ";
					cout << card[0];
					cout << "|";
				}else if(card[0] != '1' && card[1]=='c'){
					cout << "  Y  ";
					cout << card[0];
					cout << "|";
				}else if(card[0] != '1' && card[1]=='s'){
					cout << "  I  ";
					cout << card[0];
					cout << "|";
				}

			}else if(row==5)
				cout << "------'";
		
		} // end card loop

		cout << endl;

	} // end row loop

} // end displayHand()



/*
Name: displayAllHands
Description: Displays dealer hand and all player hands
Parameters:
int numPlayers: number of players
const Player &p1: Player one structure
const Player &p2: Player two structure
const vector<string> &dealerHand: Dealer's hand
bool hideHole: Tells displayHand() whether to hide hole card
Return: void
*/
void displayAllHands(int numPlayers, const Player &p1, const Player &p2, 
					 const vector<string> &dealerHand, bool hideHole)
{
	// DEALER
	cout << "DEALER'S HAND:\n";
	displayHand(dealerHand, hideHole);
	cout << setfill(' ') << endl;

	// SET FORMATTING
	cout << fixed << setprecision(2);

	// PLAYER 1
	cout << left <<  setw(20) << p1.username; 
	cout << "Money: " << left << setw(10) << p1.money;
	if(p1.insurance > 0)
		cout << "Insurance: " << left << setw(10) << p1.insurance;
	cout << endl;
	cout << "Hand 1 Bet: " << p1.bet1;
	cout << endl;
	displayHand(p1.hand1, false);
	if(p1.numHands > 1){  // two hands
		cout << "Hand 2 Bet: " << p1.bet2;
		cout << endl;
		displayHand(p1.hand2, false);
		if(p1.numHands > 2){  // three hands
			cout << "Hand 3 Bet: " << p1.bet3;
			cout << endl;
			displayHand(p1.hand3, false);
		} // end if
	} // end if
	cout << endl;

	// PLAYER 2
	if(numPlayers==2)
	{
		cout << left << setw(20) << p2.username;
		cout << "Money: " << left << setw(10) << p2.money;
		if(p2.insurance > 0)
			cout << "Insurance: " << left << setw(10) << p2.insurance;
		cout << endl;
		cout << "Hand 1 Bet: " << p2.bet1;
		cout << endl;
		displayHand(p2.hand1, false);
		if(p2.numHands > 1){  // two hands
			cout << "Hand 2 Bet: " << p2.bet2;
			cout << endl;
			displayHand(p2.hand2, false);
			if(p2.numHands > 2){  // three hands
				cout << "Hand 3 Bet: " << p2.bet3;
				cout << endl;
				displayHand(p2.hand3, false);
			} // end if
		} // end if
	} // end if
	cout << endl;

} // end displayAllHands()



/*
Name: firstChoiceSplit
Description: Gets user's first decision on a hand with identical cards
Parameters:
int numPlayers: Number of players
Player &p1: Player 1 structure
Player &p2: Player 2 structure
vector<string> &dealerHand: Dealer's hand
Player &p: Structure of player whose turn it is
int currHand: Which hand 1-3 of current player is in play
Return: int choice: Choice user makes
*/
int firstChoiceSplit(int numPlayers, Player &p1, Player &p2, vector<string> &dealerHand, Player &p, int currHand)
{
	int choice;  // a user's choice

	cout << "What would you like to do to hand " << currHand << ", " << p.username << "?\n";
	cout << "(1)Hit  (2)Stand  (3)Double Down  (4)Surrender  (5)Split\n";
	cout << "\nEnter your choice (1-5): ";
	cin >> choice;

	while(choice < 1 || choice > 5 || ((choice == 5 || choice == 3) && p.money < p.bet1)){
		system("clear");
		displayAllHands(numPlayers, p1, p2, dealerHand, true);
		if(choice < 1 || choice > 5){
			cout << "Choice must be 1-5.\n"
				 << "(1)Hit  (2)Stand  (3)Double Down  (4)Surrender  (5)Split\n"
				 << "Re-enter choice for hand " << currHand << ", " << p.username << ": ";
		}else{
			cout << "You don't have enough money to match your original bet.\n"
				 << "(1)Hit  (2)Stand  (3)Double Down  (4)Surrender  (5)Split\n"
				 << "Re-enter choice for hand " << currHand << ", " << p.username << ": ";
		}		
		cin >> choice;
	}

	return choice;

} // end firstChoiceSplit()



/*
Name: firstChoice
Description: Gets user's first choice on hand with two differently valued cards
Parameters:
int numPlayers: Number of players
Player &p1: Player 1 structure
Player &p2: Player 2 structure
vector<string> &dealerHand: Dealer's hand
Player &p: Structure of player whose turn it is
int currHand: Which hand 1-3 of current player is in play
Return: int choice: Choice user makes
*/
int firstChoice(int numPlayers, Player &p1, Player &p2, vector<string> &dealerHand, Player &p, int currHand)
{
	int choice;  // a user's choice

	cout << "What would you like to do to hand " << currHand << ", " << p.username << "?\n";
	cout << "(1)Hit  (2)Stand  (3)Double Down  (4)Surrender\n";
	cout << "\nEnter your choice (1-4): ";
	cin >> choice;

	while(choice < 1 || choice > 4 || (choice == 3 && p.money < p.bet1)){
		system("clear");
		displayAllHands(numPlayers, p1, p2, dealerHand, true);
		if(choice < 1 || choice > 4){
			cout << "Choice must be 1-4.\n"
				 << "(1)Hit  (2)Stand  (3)Double Down  (4)Surrender\n"
				 << "Re-enter choice for hand " << currHand << ", " << p.username << ": ";
		}else{
			cout << "You don't have enough money to match your original bet.\n"
				 << "(1)Hit  (2)Stand  (3)Double Down  (4)Surrender\n"
				 << "Re-enter choice for hand " << currHand << ", " << p.username << ": ";
		}		
		cin >> choice;
	}

	return choice;

} // end firstChoice()



/*
Name: choice
Description: Gets user choice on hand with 3 or more cards
Parameters:
int numPlayers: Number of players
Player &p1: Player 1 structure
Player &p2: Player 2 structure
vector<string> &dealerHand: Dealer's hand
Player &p: Structure of player whose turn it is
int currHand: Which hand 1-3 of current player is in play
Return: int choice: Choice user makes
*/
int choice(int numPlayers, Player &p1, Player &p2, vector<string> &dealerHand, Player &p, int currHand)
{
	int choice;  // a user's choice

	cout << "What would you like to do to hand " << currHand << ", " << p.username << "?\n";
	cout << "(1)Hit  (2)Stand  (3)Double Down\n";
	cout << "\nEnter your choice (1-3): ";
	cin >> choice;

	while(choice < 1 || choice > 3 || (choice == 3 && p.money < p.bet1)){
		system("clear");
		displayAllHands(numPlayers, p1, p2, dealerHand, true);
		if(choice < 1 || choice > 3){
			cout << "Choice must be 1-3.\n"
				 << "(1)Hit  (2)Stand  (3)Double Down\n"
				 << "Re-enter choice for hand " << currHand << ", " << p.username << ": ";
		}else{
			cout << "You don't have enough money to match your original bet.\n"
				 << "(1)Hit  (2)Stand  (3)Double Down\n"
				 << "Re-enter choice for hand " << currHand << ", " << p.username << ": ";
		}		
		cin >> choice;
	}

	return choice;

} // end choice()



/*
Name: payout
Description: Determines winnings for a hand, adds to money pot
and writes winnings to file on a player's record
Parameters:
Player &p: Player whose hand won
double &bet: Bet on hand that won
fstream &accounts: File stream
bool blackjack: Flag if hand won on blackjack
Return: void
*/
void payout(Player &p, double &bet, fstream &accounts, bool blackjack, bool insurance)
{
	double winnings;       // winnings for a hand
	       //totalWinnings;  // a player's all-time winnings
	bool found = false;    // flag for finding a username in accounts.txt
	// string input;          // holds input from file
	int byteNum;           // holds position of a field

	//accounts.open("accounts.txt", ios::in|ios::out); // open in read/write mode

	/*
	while(!found && accounts >> input){  // find record and read in totalWinnings
		if(p.username==input){
			found = true;
			accounts >> input;
			accounts >> input;
			byteNum = accounts.tellg();  // record read position of totalWinnings
			accounts >> totalWinnings;  
		}else{
			accounts >> input;
			accounts >> input;
			accounts >> input;
		}
	} // end while  */
	
	if(blackjack){               // determine winnings for hand
		winnings = bet * 1.5;
		p.money += (winnings + bet);
		bet = 0;
	}else if(insurance){
		winnings = bet * 2;
		p.money += (winnings + bet);
		bet = 0;
	}else{
		winnings = bet;
		p.money += (winnings + bet);
		bet = 0;
	} // end if blackjack

	// accounts.seekp((byteNum + 1), ios::beg);  // set write position
	// totalWinnings += winnings;                // add winnings to totalWinnings
	// accounts << totalWinnings;                // write total winnings to file

	//accounts.close();

} // end payout()



/*
Name: loss
Description: Subtracts loss on hand from money pot and updates
record for player in file with lost bet 
Parameters:
Player &p: Player structure
double &bet: Bet lost on current hand
fstream &accounts: file stream
bool surrender: Flags a hand surrender
Return: void
*/
void loss(Player &p, double &bet, fstream &accounts, bool surrender = false)
{
	//accounts.open("accounts.txt", ios::in|ios::out);    // open file in read/write mode

	double //totalWinnings,    // a player's total winnings
		   loss;             // player's loss for that hand
	bool found = false;      // flags username found
	string input;            // input from a file
	int byteNum;             // position of field to be updated

	/*
	while(!found && accounts >> input){  // find record and read in totalWinnings
		if(p.username==input){
			found = true;
			accounts >> input;
			accounts >> input;
			byteNum = accounts.tellg();  // record read position of totalWinnings
			accounts >> totalWinnings;  
		}else{
			accounts >> input;
			accounts >> input;
			accounts >> input;
		}
	} // end while  */

	if(surrender){
		loss = bet / 2;
		p.money += loss;
		bet = 0;
	}else{
		loss = bet;
		bet = 0;
	}

	//totalWinnings -= loss;                    // update totalWinnings
	//accounts.seekp((byteNum + 1), ios::beg);  // set write position
	//accounts << totalWinnings;                // update file with totalWinnings

	//accounts.close();

} // end loss()



/*
Name: placeBet
Description: Asks player to place bet on hand
Parameters:
Player &p: Player structure for updating bet1 and money
Return: void
*/
void placeBet(Player &p){
	cout << p.username << ", you have " << p.money << " in chips.";
	cout << "\nEnter your bet: ";
	cin >> p.bet1;
	while(p.bet1 <=0 || p.bet1 > p.money){  // validate
		cout << "\nBet must not be negative, zero or greater than your chips.";
		cout << "\nRe-enter bet: ";
		cin >> p.bet1;
	}
	p.money = p.money - p.bet1;  // take bet out of money pot

} // end placeBet()



/*
Name: getHandValue
Description: Determines value of a hand
Parameters:
const vector<string> &hand: String vector of cards
Return: int (numeric value of hand)
*/
int getHandValue(vector<string> &hand)
{
	int total = 0;         // hand total value
	bool hasAce = false;   // if hand contains an Ace

	for(int i=0; i < hand.size(); i++){
		if(getCardValue(hand[i]) == 1)  // if hand has ace, set flag
			hasAce = true;

		total += getCardValue(hand[i]);  // increment total of hand
	} // end for

	if(hasAce)
		if((total + 10) <= 21) // use one ace as 11 only if hand won't bust
			total += 10;

	return total;

} // end getHandValue()



/*
Name: getCardValue
Description: Determines value of a card
Parameters:
string card: String such as "Ah" or "4c" representing a card
Return: int (card's value)
*/
int getCardValue(string card)
{
	int value = 0;   // value of card

	if(card[0] == 'A')
		value = 1;  // value of 11 only applied in getHandValue()
	else if(card[0] == '2')
		value = 2;
	else if(card[0] == '3')
		value = 3;
	else if(card[0] == '4')
		value = 4;
	else if(card[0] == '5')
		value = 5;
	else if(card[0] == '6')
		value = 6;
	else if(card[0] == '7')
		value = 7;
	else if(card[0] == '8')
		value = 8;
	else if(card[0] == '9')
		value = 9;
	// '1' is first char of 10 card
	else if(card[0] == '1' || card[0] == 'J' || card[0] == 'Q' || card[0] == 'K')
		value = 10;

	return value;

} // end getCardValue()



/*
Name: dealCard
Description: Take card from deck and puts it in a hand
Parameters:
vector<string> &deck: Deck vector
vector<string> &hand: Hand to be dealt to
Return: void
*/
void dealCard(vector<string> &deck, vector<string> &hand)
{
	string card;  // to hold card from deck

	card = deck[deck.size() - 1];  // assign card last value in deck vector
	deck.pop_back();               // pop last value in deck back
	hand.push_back(card);          // put card into hand

} // end dealCard()


