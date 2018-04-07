/* KMeansClustering.cpp : Defines the entry point for the console application.
** Completed by Jonathon Teague on 3/28/18
** Implements the K-means clustering algorithm on numerical data in CSV format
** Limitations of this algorithm are fairly severe:
**    - Requires completeness of data (no missing values)
**    - Cannot interact with categorical variables
**    - Converting Boolean variables to 0 and 1 gives them unfair weight
** Program implements error handling in cases of wrong user input- terminates upon certain unrecoverable errors
** The algorithm was alpha-tested with UCI machine learning's wine data set: https://archive.ics.uci.edu/ml/machine-learning-databases/wine/
** As well as the class iris data set: https://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data
** See also https://archive.ics.uci.edu/ml/machine-learning-databases/breast-cancer-wisconsin/wdbc.data
** Reference for algorithm: http://mnemstudio.org/clustering-k-means-example-1.htm
*/

#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <string>
#include "K-Means.h"
#include "K-Util.h"

int main()
{
	RD rd;
	Kmeans* kmeans = NULL;
	Kmeans* nom_kmeans = NULL;
	// Collect necessary information for the file/testing variable
	int no_points = 0;
	int err = 100;
	int r_points = 0;
	bool remd;
	bool nom;
	bool test = false;
	std::vector<int>* prediction = new std::vector<int>;
	int p_points = 0;
	Plist* head = new Plist;
	Plist* phead = new Plist;
	Plist* nhead = new Plist;
	Plist* rhead = NULL;
	std::ifstream input;
	std::ofstream dummy;
	std::string inp, ifname, tfname;
	tfname = "";
	inp = "New";
	while (true) {
		if (err == 100) { // Program start
			reset_Kmeans(p_points, r_points, rhead, phead, nhead, prediction, head, rd, no_points, kmeans, nom_kmeans);
			r_points = 0;
			remd = false;
			test = false;
			nom = false;
			err = get_rdata(no_points, r_points, rhead, head, nhead, ifname, input, err, kmeans, rd);
			if (err < 0) continue;
			std::cout << "Performing K-Means.. If the program takes more than a minute to run, the learning set may be too large.\n";
			if (!run(kmeans, nom_kmeans, no_points, head, nhead, rd)) {err = -6; continue;}
			if (rd.var1 > -1 && r_points > 0) {
				test = true;
				std::cout << std::string(50, '\n');
				if (r_points > 0) {
					err = predict(r_points, rhead, tfname, input, prediction, err, remd, nom_kmeans, rd);
					disp_predictions(*prediction, r_points, rhead, remd, nom_kmeans, true, false, dummy, rd);
				}
				std::cout << "Press any key to continue, or 'Log' to log the results.\n";
				std::cin.clear();
				std::getline(std::cin, inp);
				if (inp[0] == 'l' || inp[0] == 'L') {
					Log_f(no_points, r_points, kmeans, nom_kmeans, rd, rhead, remd, true, true, true, ifname, tfname, prediction);
					std::cout << "Press any key to continue or type 'Quit' to exit..\n";
					std::cin.clear();
					std::getline(std::cin, inp);
					if (inp[0] == 'q' || inp[0] == 'Q') return 0;
				}
			}
		}
		// Error checking
		if (err == -100) return 0;
		if (err == -1 || err == -4) {
			std::cout << "Failed to open file: " << ifname << ". Type 'Enter' to try again or 'Quit' to quit\n";
			std::cin.clear();
			std::getline(std::cin, inp);
			if (inp[0] == 'q' || inp[0] == 'Q') return 0;
			else err = 100;
			continue;
		}
		else if (err == -2 || err == -5) {
			std::cout << "Invalid input. Did you include categorical (non-numeric) data?\nType 'Quit' to exit the program or press 'Enter' to try a new file.\n";
			std::cin.clear();
			std::cin.sync();
			std::getline(std::cin, inp);
			if (inp[0] == 'q' || inp[0] == 'Q') return 0;
			else err = 100;
			continue;
		}
		else if (err == -3) {
			std::cout << "Type 'Quit' to exit the program or press 'Enter' to try a new file.\n";
			std::cin.clear();
			std::cin.sync();
			std::getline(std::cin, inp);
			if (inp[0] == 'q' || inp[0] == 'Q') return 0;
			err = 100;
			continue;
		}
		else if (err == -6) { // Priority error: unrecoverable (not handled)
			std::cout << "Failure to run on data set " << ifname << ": Program terminating.\nPress any key to exit\n";
			std::cin.clear();
			std::cin.sync();
			std::getline(std::cin, inp);
			return 0;
		}
		// Talk to user
		std::cout << std::string(50, '\n');
		if (err < 0)
			if (inp[0] != 'n' && inp[0] != 'N') {
				continue;
			}
		if (inp[0] == 'n' || inp[0] == 'N') {
			if (no_points < 1) {
				err = -6;
				continue;
			}
			else std::cout << no_points << " data entries were read. ";
		}
		else if (inp[0] == 'p' || inp[0] == 'P') {
			if (p_points > 1) std::cout << p_points << " data entries were placed into predicted categories. ";
			else if (p_points == 1) std::cout << p_points << " data entry was placed into a predicted category. ";
		}
		std::cout << "Choose from the following menu options:\n";
		if (rd.var1 > -1 && p_points > 0 && err > -10) std::cout << "'Results' or 'R' to view results of predictions\n";
		else if (rd.var1 == -1 && p_points > 0 && err > -10) std::cout << "'Results' or 'R' to view results of placements\n";
		if (err > -10 && no_points > 0)std::cout << "'Display' or 'D' to display results of the program run\n";
		if(err > -10 && no_points > 0 && rd.var1 > -1) std::cout << "'Spread' or 'S' to show the spread of data elements in each cluster (corresponding to line in file)\n";
		if(rd.var1 > -1 && err > -10 && no_points > 0)std::cout << "'Predict' or 'P' to read a file and make predictions\n";
		else if (err > -10 && no_points > 0)std::cout << "'Place' or 'P' to read a file and find which groups the points cluster to\n";
		if (err > -10 && no_points > 0)std::cout << "'Log' or 'L' to log the results of this run to a file\n";
		std::cout << "'New' or 'N' to run a K-Means on a new data set\n";
		std::cout << "'Quit' or 'Q' to exit the program\n";
		std::cin.clear(); 
		std::cin.sync();
		std::getline(std::cin, inp);
		if (inp[0] == 'q' || inp[0] == 'Q') return 0;
		if (inp[0] == 'd' || inp[0] == 'D' || inp[0] == 'p' || inp[0] == 'P' || inp[0] == 's' || inp[0] == 'S') {
			char t = inp[0];
			if(t == 'd' || t == 'D') std::cout << "Display the nominalized ('n') or regular ('r') data set?\n";
			else if(t == 'p' || t == 'P') {
				delete prediction;
				prediction = new std::vector<int>;
				std::cout << "Predict from the nominalized ('n') or regular ('r') data set?\n";
			}
			else {
				std::cout << "Show results of the nominalized ('n') or regular ('r') data set?\n";
			}
			std::cin.clear();
			std::cin.sync();
			std::getline(std::cin, inp);
			if (inp[0] == 'n' || inp[0] == 'N') { nom = true; inp[0] = t; inp.append("n"); }
			else if (inp[0] == 'r' || inp[0] == 'R') { nom = false; inp[0] = t; inp.append("r"); }
			else { inp = ""; continue; }
		}
		std::cout << std::string(50, '\n');
		switch (inp[0]) {
		case 'd':
		case 'D': if (inp[1] == 'r') { inp = disp_res(no_points, kmeans, rd, dummy, false, test, inp); break; }
				  else if (inp[1] == 'n') { inp = disp_res(no_points, nom_kmeans, rd, dummy, false, test, inp); break; }
				  break;
		case 's':
		case 'S': if (rd.var1 > -1) {
			if (inp[1] == 'r' || inp[1] == 'R') { show_spread(no_points, kmeans, rd, dummy, false, ifname); break; }
			else if (inp[1] == 'n' || inp[1] == 'N') { show_spread(no_points, nom_kmeans, rd, dummy, false, ifname); break; }
			else continue;
		}
				 else continue;
		case 'P':
		case 'p': p_points = 0;
				  if (inp[1] == 'r') err = predict(p_points, phead, tfname, input, prediction, err, remd, kmeans, rd);
				  else if (inp[1] == 'n') err = predict(p_points, phead, tfname, input, prediction, err, remd, nom_kmeans, rd);
				  if (err < 0) std::cout << "Unexpected error in making predictions.\n";
				  else if (err == 5) inp = "Failed";
				  break;
		case 'r':
		case 'R': if (!prediction->empty()) {
			if (!nom) disp_predictions(*prediction, p_points, phead, remd, kmeans, false, false, dummy, rd);
			else disp_predictions(*prediction, p_points, phead, remd, nom_kmeans, false, false, dummy, rd);
			break;
		}
				  else { std::cout << "You must make predictions before viewing them!\n";  break; }
		case 'L':
		case 'l': if (tfname != "") Log_f(no_points, p_points, kmeans, nom_kmeans, rd, phead, false, remd, false, nom, ifname, tfname, prediction);
				  else Log_f(no_points, p_points, kmeans, nom_kmeans, rd, phead, false, false, false, nom, ifname, tfname, prediction);
				  break;
		case 'n':
		case 'N': err = 100; continue;
		case 'q':
		case 'Q': return 0;
		default: continue;
		}
		if (inp[0] == 'n' || inp[0] == 'N' || inp[0] == 'p' || inp[0] == 'P' || inp[0] == 'F') continue;
		std::cout << "Press any key to continue or type 'Quit' to exit..\n";
		std::cin.clear();
		std::cin.sync();
		std::getline(std::cin, inp);
		if (inp[0] == 'q' || inp[0] == 'Q') return 0;
	}
	return 0;
}
