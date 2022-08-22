/*
 * @file botTemplate.cpp
 * @author Arun Tejasvi Chaganty <arunchaganty@gmail.com>
 * @date 2010-02-04
 * Template for users to create their own bots
 *
 * Template modified by CS20M070 - Vikram Manjare, IITM for the programming assignment 2 of AI-SMPS course
 */

#include "Othello.h"
#include "OthelloBoard.h"
#include "OthelloPlayer.h"
#include <cstdlib>
#include <queue>
#include <climits>
using namespace std;
using namespace Desdemona;

class MyBot: public OthelloPlayer {
private:
	int LOW = -5000, HIGH = 5000;
	Turn ourTurn;
	int MAX_DEPTH = 3;

	bool isCornerMove(Move mov) {
		int a[] = { 0,0,7,7 };
		int b[] = { 0,7,0,7 };
		int size = 1;
		for (int i = 0; i < size; i++) {
			if (mov.x == a[i] && mov.y == b[i]) {
				return true;
			}
		}
		return false;
	}

	int getProfit(const OthelloBoard &board, Turn nextTurn) {
		int heuristics = board.getValidMoves(nextTurn).size() * 10;
		if (nextTurn != ourTurn) {
			heuristics *= -1;
		}
		if (ourTurn == RED) {
			heuristics += board.getRedCount() - board.getBlackCount();
		} else {
			heuristics += board.getBlackCount() - board.getRedCount();
		}
		return heuristics;
	}

	struct Node {
		int key;
		int childKey;
		Node *parent;
		vector<Node*> child;
	};

	Node* newNode(int key) {
		Node *temp = new Node;
		temp->key = key;
		return temp;
	}

	int alphaBeta(const OthelloBoard &board, Node *parent, int depth,
			bool isOurTurn, int alpha, int beta);
public:
	/**
	 * Initialisation routines here
	 * This could do anything from open up a cache of "best moves" to
	 * spawning a background processing thread.
	 */
	MyBot(Turn turn);

	/**
	 * Play something
	 */
	virtual Move play(const OthelloBoard &board);
};

MyBot::MyBot(Turn turn) :
		OthelloPlayer(turn) {
	ourTurn = turn;
}

Move MyBot::play(const OthelloBoard &board) {
	list<Move> moves = board.getValidMoves(ourTurn);

	list<Move>::iterator it = moves.begin();

	Move bestMove = *it;
	Node *root = newNode(LOW);
	root->childKey = LOW;

	int beta = HIGH;
	for (Move move : moves) {
		OthelloBoard tempBoard = board;
		tempBoard.makeMove(ourTurn, move);

		Node *tempRoot = newNode(HIGH);
		tempRoot->childKey = HIGH;
		(root->child).push_back(tempRoot);

		int newValue = alphaBeta(tempBoard, tempRoot, MAX_DEPTH - 1, false,
				root->childKey, beta);
		if (isCornerMove(move)) {
			newValue += 100;
		}
		if (newValue > root->childKey) {
			root->childKey = newValue;
			bestMove = move;
		}
	}

	return bestMove;
}

int MyBot::alphaBeta(const OthelloBoard &board, Node *parent, int depth,
		bool isOurTurn, int alpha, int beta) {
	Turn currentTurn = isOurTurn ? ourTurn : ourTurn == RED ? BLACK : RED;

	list<Move> moves = board.getValidMoves(currentTurn);
	if (moves.size() == 0 || depth == 0) {
		return getProfit(board, currentTurn);
	}

	if (isOurTurn) {
		int value = LOW;
		for (Move move : moves) {
			OthelloBoard tempBoard = board;
			tempBoard.makeMove(currentTurn, move);

			Node *tempRoot = newNode(HIGH);
			tempRoot->childKey = HIGH;
			(parent->child).push_back(tempRoot);

			int newValue = alphaBeta(tempBoard, tempRoot, depth - 1, false,
					alpha, beta);
			if (isCornerMove(move)) {
				newValue += 100;
			}
			if (depth == 1) {
				tempRoot->childKey = newValue;
			}
			bool isUpdate = newValue > parent->childKey;
			if (isUpdate) {
				parent->childKey = newValue;
				value = newValue;
			}
			if (newValue >= beta) {
				break;
			}
			if (isUpdate)
				alpha = newValue;
		}
		return value;

	} else {
		int value = HIGH;
		for (Move move : moves) {
			OthelloBoard tempBoard = board;
			tempBoard.makeMove(currentTurn, move);

			Node *tempRoot = newNode(LOW);
			tempRoot->childKey = LOW;
			(parent->child).push_back(tempRoot);

			int newValue = alphaBeta(tempBoard, tempRoot, depth - 1, true,
					alpha, beta);
			if (isCornerMove(move)) {
				newValue -= 100;
			}
			if (depth == 1) {
				tempRoot->childKey = newValue;
			}
			bool isUpdate = newValue < parent->childKey;
			if (isUpdate) {
				parent->childKey = newValue;
				value = newValue;
			}
			if (newValue <= alpha) {
				break;
			}
			if (isUpdate)
				beta = newValue;
		}
		return value;
	}
}

// The following lines are _very_ important to create a bot module for Desdemona

extern "C" {
OthelloPlayer* createBot(Turn turn) {
	return new MyBot(turn);
}

void destroyBot(OthelloPlayer *bot) {
	delete bot;
}
}

