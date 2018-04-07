/* KmeansUtil.cpp : Defines the entry point for the console application.
** Completed by Jonathon Teague on 3/28/18
** Implements various display and run function calls
*/
#include <iomanip>
#include <string>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <cstdio>
#include <vector>
#include <math.h>
#include "K-Means.h"
#include "K-Util.h"

// Display function for the spread of points in clusters and within the clusters corresponding to the categorical variable- only available if given a variable to test on
void show_spread(int no_points, Kmeans*& kmeans, RD& rd, std::ofstream& logfile, bool log, std::string fname) {
	if (!(log && !kmeans->nominalized)) {
		if (log) logfile << "The results of performing K-means on " << fname << ":\n";
		else std::cout << "The results of performing K-means on " << fname << ":\n";
		for (int i = 0; i < rd.unique; i++) {
			int t = kmeans->categories.at(i);
			if (t == no_points) {
				if (!log) std::cout << "There were " << kmeans->categories.at(i) << " cases of variable '" << kmeans->typez.at(i) << "' out of " << no_points << " (100.00%)\n";
				else logfile << "There were " << kmeans->categories.at(i) << " cases of variable '" << kmeans->typez.at(i) << "' out of " << no_points << " (100.00%)\n";
			}
			else {
				double r = ((double)t / (double)no_points) * 100;
				if (!log) std::cout << "There are " << kmeans->categories.at(i) << " cases of variable '" << kmeans->typez.at(i) << "' out of " << no_points << " (";
				else logfile << "There are " << kmeans->categories.at(i) << " cases of variable '" << kmeans->typez.at(i) << "' out of " << no_points << " (";
				if (!log) std::cout << std::fixed << std::setprecision(2) << r << "%)\n";
				else logfile << std::fixed << std::setprecision(2) << r << "%)\n";
			}
		}
		logfile << "-------------------------------------------------------------------------------------------------------\nThe nominalized set predicts a precision of " << kmeans->precision << "%.\n\n";
	}
	else if (log) logfile << "-------------------------------------------------------------------------------------------------------\nThe standard set predicts a precision of " << std::fixed << std::setprecision(2) << kmeans->precision << "%.\n\n";
	else if (!log)
		if (kmeans->nominalized) std::cout << std::fixed << std::setprecision(2) << "The nominalized set predicts a precision of " << kmeans->precision << ".\n\n";
		else std::cout << std::fixed << std::setprecision(2) << "The standard set predicts a precision of " << kmeans->precision << ".\n\n";
	for (int i = 0; i < rd.K; i++) {
		if (!log) std::cout << "The spread of Cluster " << i + 1 << " is:\n";
		else logfile << "The spread of Cluster " << i + 1 << " is:\n";
		if (kmeans->clusters[i]->get_no_points() < 1) {
			if (log) logfile << "This cluster is empty.\n";
			else std::cout << "This cluster is empty.\n";
		}
		else for (int k = 0; k < rd.K; k++) {
			int t = kmeans->clusters[i]->get_typed(k);
			int y = kmeans->clusters[i]->get_no_points();
			if (t == 0) {
				if (!log) std::cout << "Variable '" << kmeans->typez.at(k) << "' represents " << t << " out of " << y << " points (0.00%)\n";
				else logfile << "Variable '" << kmeans->typez.at(k) << "' represents " << t << " out of " << y << " points (0.00%)\n";
			}
			else if (t == y) {
				if (!log) std::cout << "Variable '" << kmeans->typez.at(k) << "' represents " << t << " out of " << y << " points (100.00%)\n";
				else logfile << "Variable '" << kmeans->typez.at(k) << "' represents " << t << " out of " << y << " points (100.00%)\n";
			}
			else {
				double r = ((double)t / (double)y) * 100;
				if (!log) std::cout << "Variable '" << kmeans->typez.at(k) << "' represents " << t << " out of " << y << " points (" << r << "%)\n";
				else logfile << "Variable '" << kmeans->typez.at(k) << "' represents " << t << " out of " << y << " points (" << r << "%)\n";
			}
		}
	}
}

// Painstakingly created (<3) display function to neatly display the members of clusters and their corresponding means per variable
std::string disp_res(int no_points, Kmeans*& kmeans, RD& rd, std::ofstream& logfile, bool log, bool test, std::string inp) {
	std::string spc = "              ";
	std::string spc1 = "";
	std::string spc2 = "";
	int cf = 0;
	double vf = 0;
	int dig = 0;
	double f1 = 0;
	double f2 = 0;
	for (int y = 0; y < rd.K; y++){
		for (int z = 0; z < rd.no_fields; z++){
			if(z == rd.var1) continue;
			f1 = kmeans->clusters[y]->get_meanz(z);
			if(f1 < 0){
			       f1 = abs(f1) * 10;
			       if(f1 < 10) f1 = 10;
			}
			if(f2 < f1) f2 = f1;
		}
	}
	while (f2 > 9) {
		vf++;
		spc1 = spc1 + " ";
		f2 = f2 / 10;
	}
	if(kmeans->named)
		if(vf < 7) vf = 7;
		else dig = vf - 7;
	std::cout << vf << " vf\n";
	vf = pow(10, vf);
	int tmp;
	if(!(kmeans->named)){
	tmp = rd.no_fields;
		while (tmp > 9) {
			dig++;
			spc2 = spc2 + " ";
			tmp = tmp / 10;
		}
	}
	dig = pow(10, dig);
	std::cout << dig << " dig\n";
	tmp = no_points;
	while (tmp>9) {
		tmp = tmp / 10;
		cf++;
	}
	cf = pow(10,cf);
	if (!log)
		if (kmeans->nominalized) std::cout << "The nominalized set found the following sets of data:";
		else std::cout << "The standard set found the following sets of data:";
	int j = 0;
	for (int i = 0; i < rd.K; i++) {
		bool typed = false;
		j = 0;
		if (!log) std::cout << "\n\nThe K-Means of Cluster no." << i + 1 << " are:";
		else logfile << "\n\nThe K-Means of Cluster no." << i + 1 << " are:";
		if (kmeans->clusters[i]->get_no_points() < 1) {
			if (log) logfile << "\nThis cluster is empty.";
			else std::cout << "\nThis cluster is empty.";
		}
		else while (j < rd.no_fields / 4 + 1) {
			if ((rd.var1 > -1 && (j * 4) == rd.no_fields) || (rd.var1 > -1 && (j * 4 +1 == rd.no_fields))) break;
			else if (rd.var1 == -1 && j * 4 == rd.no_fields) break;
			if (!log) std::cout << "\nVariables:" << spc;
			else logfile << "\nVariables:" << spc;
			for (int r = 0; r < 4; r++) {
				int x_x = j * 4 + r;
				if (x_x == rd.var1) {
					typed = true;
					x_x++;
				}
				else if (typed) x_x++;
				if (x_x == rd.no_fields) break;
				if(!(kmeans->named)){
					std::string nspc = spc1 + "    ";
					if(rd.negz) nspc = nspc + "  ";
					int xp = x_x+1;
					if(xp % 10 == 0) xp++;
					if(xp <2) xp=2; 
					while(dig / xp > 0){
						nspc = nspc + " ";
						xp = xp*10;
					}
					if (!log) std::cout << x_x + 1 << spc << spc1 << nspc << " ";
					else logfile << x_x + 1 << spc << spc1 << nspc << " ";
				}
				else {
					std::string nspc= "";
					if(rd.negz > 0) nspc = nspc + " ";
					if (!log) std::cout << kmeans->catz.at(x_x) << spc << nspc;
					else logfile << kmeans->catz.at(x_x) << spc << nspc;
				}
			}
			if (!log) std::cout << "\nVar-Mean: " << spc;
			else logfile << "\nVar-Mean: " << spc;
			for (int r = 0; r < 4; r++) {
				int x_x = j * 4 + r;
				if(typed) x_x++;
				if (x_x == rd.no_fields) break;
				double xP = kmeans->clusters[i]->get_meanz(x_x);
				double Dx = xP;
				std::string nspc = spc2;
				if(kmeans->named) nspc =nspc;
				if(Dx >= 0) nspc = nspc + " ";
				else if(Dx < 0) Dx = abs(Dx);
				if(Dx <= 1) Dx = 1.1;
				while(vf/ Dx >= 1){
					nspc = nspc + " ";
					Dx = Dx * 10;
				}
				if (!log) std::cout << std::fixed << std::setprecision(4) << xP <<spc << spc2 << nspc;
				else logfile << std::fixed << std::setprecision(4) << xP << spc << spc2 << nspc;
				}
			j++;
		}
	}

	if (!log) {
		std::cout << "\nType 'More' or 'M' to see more information about the points within each cluster.\nPress Enter to return to the main menu.\n";
		std::getline(std::cin, inp);
	}
	if (inp[0] == 'm' || inp[0] == 'M') {
		for (int i = 0; i < rd.K; i++) {
			if (!log) std::cout << "\n\nClustered in Cluster no." << i + 1 << ":";
			else logfile << "\n\nClustered in Cluster no." << i + 1 << ":";
			if (kmeans->clusters[i]->get_no_points() > 0) for (int k = 0; k < kmeans->clusters[i]->get_no_points(); k++) {
				if (k % 10 == 0) if (!log) std::cout << "\n";
				else logfile << "\n";
				int x_x = kmeans->clusters[i]->get_points().at(k).get_id() + 1;
				if (test && x_x % 10 == 0) x_x--;
				int vx = x_x;
				if(vx %10 == 0) vx++;
				std::string nspc = "  ";
				while(cf / vx){
					nspc = nspc + " ";
					vx = vx * 10;
				}
				if (!log) std::cout << x_x << nspc;
				else logfile << x_x << nspc;
			}
			else {
				if (log) logfile << "\nThere are no points are in this cluster.";
				else std::cout << "\nThere are no points are in this cluster.";
			}
			if (log) logfile << "\n";
			else std::cout << "\n";
		}
		if (!log) return "Disp";
		else return "Log";
	}
	else if (!log) {
		std::cout << "\n"; return "Disp";
	}
	else {
		logfile << "\n"; return "Log";
	}
} 

// Check if file exists before writing to it
bool of_exists(std::string ofname) {
	std::FILE * test;
	if (!(test = fopen(ofname.c_str(), "r"))) {
		return false;
	}
	std::cout << "File " << ofname << " already exists!\n";
	fclose(test);
	return true;
}

// Ask user if they want to quit the program after bad input
void Log_resp(std::string& inp) {
	std::cout << "Type 'Return' to return to the main menu, Press 'Enter' to try again, or 'Quit' to exit the program\n";
	std::cin.clear();
	std::getline(std::cin, inp);
}

// Write runtime data to a given file- can overwrite if user confirms
// Calls show_spread, disp_res and disp_predictions if relevant
int Log_f(int no_points, int p_points, Kmeans*& kmeans, Kmeans*& nom_kmeans, RD& rd, Plist*& phead, bool part, bool remd, bool test, bool nom, std::string& ifname, std::string& pfname, std::vector<int>*& predictions){
	std::string inp, ofname, j;
	while (true) {
		std::cout << "Enter the name of the file you would log this run to.\n";
		std::cin.clear();
		std::getline(std::cin, ofname);
		ofname = ofname + ".txt";
		if (of_exists(ofname)) {
			std::cout << "File named " << ofname << " already exists. Overwrite? (Y/N)\n";
			std::cin.clear();
			std::getline(std::cin, inp);
			if (inp[0] != 'Y' && inp[0] != 'y') {
				Log_resp(inp);
				if (inp[0] == 'R' || inp[0] == 'r') return 0;
				else if (inp[0] == 'Q' || inp[0] == 'q') return -1;
				else continue;
			}
			else {
				std::cout << "Delete the contents of " << ofname << " and overwrite? (This action is irreversible)\n";
				std::cin.clear();
				std::getline(std::cin, inp);
				if (inp[0] != 'Y' && inp[0] != 'y') {
					Log_resp(inp);
					if (inp[0] == 'R' || inp[0] == 'r') return 0;
					else if (inp[0] == 'Q' || inp[0] == 'q') return -1;
					else continue;
				}
				else {
					if (std::remove(ofname.c_str()) != 0) {
						std::cout << "Failed to remove " << ofname << "!\nReturning to main menu (press any key to continue)\n";
						std::cin.clear();
						std::getline(std::cin, inp);
						return 0;
					}
					else std::cout << ofname << " was successfully deleted. Overwriting it now..\n";
				}
			}
		}
		std::ofstream logfile(ofname, std::fstream::trunc);
		logfile.open(ofname);
		if(!logfile.is_open()){
			std::cout << "Failed to open logfile: " << ofname << " for writing to.\n";
			Log_resp(inp);
			if (inp[0] == 'R' || inp[0] == 'r') return 0;
			else if (inp[0] == 'Q' || inp[0] == 'q') return -1;
			else continue;
		}
		// Random error bit in logfile.fail() triggered (I believe incorrectly) on the open call. So we clear if that is the only error bit and return otherwise
		if (!logfile.good()) {
			if (!logfile.fail()) {
				std::cout << "Failed to open file " << ofname << "\n";
				return 0;
			}
			logfile.clear();
		}
		if(no_points * rd.K > 200) std::cout << "Type 'More' or 'M' to log more information about each point within each cluster.";
		else inp = "blank";
		std::cin.clear();
		std::getline(std::cin, inp);
		std::cout << "Writing to " << ofname << ". Depending on the data size, this could take a while..\n";
		if (p_points > 0) {
			logfile << predictions->size() << " predictions were made from " << pfname << " with the following results:\n";
			if (!nom) disp_predictions(*predictions, p_points, phead, remd, kmeans, part, true, logfile, rd);
			else disp_predictions(*predictions, p_points, phead, remd, nom_kmeans, part, true, logfile, rd);
			logfile << "-------------------------------------------------------------------------------------------------------\n";
		}
		if(rd.var1 > -1) show_spread(no_points, nom_kmeans, rd, logfile, true, ifname);
		else logfile << "The results of performing K-means on " << ifname << ":\n-------------------------------------------------------------------------------------------------------\nThe nominalized set contained:";
		j = disp_res(no_points, nom_kmeans, rd, logfile, true, test, inp);
		if(rd.var1 > -1) show_spread(no_points, kmeans, rd, logfile, true, ifname);
		else logfile << "-------------------------------------------------------------------------------------------------------\nThe standard set contained:";
		j = disp_res(no_points, kmeans, rd, logfile, true, test, inp);
		std::cout << "Successfully wrote to " << ofname << "!\n";
		logfile.close();
		break;
	}
	return 1;
}

// Displays results of predictions (or cluster placements if no categorical variable given in test file)
void disp_predictions(std::vector<int> predictions, int p_points, Plist*& head, bool remd, Kmeans*& kmeans, bool part, bool log, std::ofstream& logfile, RD& rd) {
	bool log_all = true;
	if (p_points > 10) {
		std::string inp;
		while (true) {
			if(log) std::cout << "More than 10 predictions were made. Log all of their results?(Y/N)\n";
			else std::cout << "More than 10 predictions were made. Display all of their results?(Y/N)\n";
			std::cin.clear();
			std::getline(std::cin, inp);
			if (inp[0] == 'Y' || inp[0] == 'y') break;
			else if (inp[0] == 'N' || inp[0] == 'n') {
				log_all = false;
				break;
			}
		}
	}
	int tmp = p_points;
	int* match = new int[rd.unique];
	int* guess = new int[rd.unique];
	for (int i = 0; i < rd.unique; i++) {
		match[i] = 0;
		guess[i] = 0;
	}
	int cf = 0;
	int correct = 0;
	while (true) {
		if (tmp < 9) break;
		tmp = tmp / 10;
		cf++;
	}
	cf = pow(10, cf);
	if (p_points > 1) {
		if (rd.var1 > -1 && !log) {
			std::cout << p_points << " predictions were made using ";
			if (kmeans->nominalized) std::cout << "nominalized data with the following results: \n";
			else std::cout << "standard data with the following results: \n";
		}
		else if (!log) std::cout << p_points << " points were placed into the following clusters:\n";
	}
	else {
		if (!log) {
			std::cout << p_points << " prediction was made using ";
			if (kmeans->nominalized) std::cout << "nominalized data with the following result: \n";
			else std::cout << "standard data with the following result: \n";
		}
	}
	Plist* temp = head;
	double x_x;
	for (int i = 0; i < p_points; i++) {
		if (log_all) {
			if (!part) {
				if (log) logfile << "Point " << i + 1;
				else std::cout << "Point " << i + 1;
			}
			else {
				if (log) logfile << "Point " << (i + 1) * 10;
				else std::cout << "Point " << (i + 1) * 10;
			}
			x_x = i + 1;
			while (x_x < cf) {
				if (log) logfile << " ";
				else std::cout << " ";
				x_x = x_x * 10;
			}
		}
		int dex = predictions.at(i);
		double chance;
		if (kmeans->clusters[temp->point->get_cluster()]->get_typed(dex) != 0) {
			chance = (double)(kmeans->clusters[temp->point->get_cluster()]->get_typed(dex));
			chance = (double)(chance / kmeans->categories.at(dex)) * 100;
			if (chance > 20) {
				double t = chance/2;
				chance = (t * Adjust_prob(temp->point, dex, kmeans, rd)) + t;
			}
		}
		else chance = 0.0;
		if (chance == 100) chance = 99.9;
		if (log_all) {
			if (log) {
				if (rd.var1 > -1 && chance > 10) logfile << " was predicted to fall under group: " << kmeans->typez.at(dex) << " with a " << std::fixed << std::setprecision(2) << chance << "% probability";
				else if (rd.var1 > -1) logfile << " was predicted to fall under group: " << kmeans->typez.at(dex) << " with less than a 10% probability";
				else logfile << " was matched to Cluster: " << temp->point->get_cluster() + 1 << "\n";
			}
			else {
				if (rd.var1 > -1 && chance > 10) std::cout << " was predicted to fall under group: " << kmeans->typez.at(dex) << " with a " << std::fixed << std::setprecision(2) << chance << "% probability";
				else if (rd.var1 > -1) std::cout << " was predicted to fall under group: " << kmeans->typez.at(dex) << " with less than a 10% probability";
				else std::cout << " was matched to Cluster: " << temp->point->get_cluster() + 1 << "\n";
			}
			if (!remd) {
				if (!log) std::cout << "\n";
				else logfile << "\n";
			}
		}
		if (rd.var1 > -1) {
			if (remd) {
				guess[temp->point->get_type()]++;
				if (temp->point->get_type() == dex) {
					match[dex]++;
					correct++;
					if (log_all) 
						if (log) logfile << " (Correct!)\n";
						else std::cout << " (Correct!)\n";
				}
				else if (log_all) {
					if (log) logfile << " (Was Group: " << kmeans->typez.at(temp->point->get_type()) << ")\n";
					else std::cout << " (Was Group: " << kmeans->typez.at(temp->point->get_type()) << ")\n";
				}
			}
		}
		temp = temp->nxt;
	}
	if (log) logfile << "------------------------------------------------------------------------------------------------------\n|                                               Results                                               |\n------------------------------------------------------------------------------------------------------\n";
	else std::cout << "------------------------------------------------------------------------------------------------------\n|                                               Results                                               |\n------------------------------------------------------------------------------------------------------\n";
	if (rd.var1 > -1) {
		if (remd) {
			if (log) {
				for (int l = 0; l < rd.unique; l++) {
					double c;
					if (kmeans->categories.at(l) == match[l]) c = 100.00;
					else c = (double)match[l] / guess[l] * 100;
					if(guess[l] > 0) logfile << "Variable '" << kmeans->typez[l] << "' was identified: " << match[l] << "/" << guess[l] << " times (" << c << "%)\n";
				}
				if (correct != p_points) logfile << std::setprecision(2) << std::fixed << "Accuracy: Correctly identified " << correct << " out of " << p_points << " (" << ((double)correct / p_points) * 100 << "%)\n";
				else logfile << std::setprecision(2) << std::fixed << "Accuracy: Correctly identified " << correct << " out of " << p_points << " (100.00%)\n";
			}
			else {
				for (int l = 0; l < rd.unique; l++) {
					double c;
					if (kmeans->categories.at(l) == match[l]) c = 100.00;
					else c = (double)match[l] / guess[l] * 100;
					if (guess[l] > 0) std::cout << "Variable '" << kmeans->typez[l] << "' was identified: " << match[l] << "/" << guess[l] << " times (" << c << "%)\n";
				}
				if (correct != p_points) std::cout << std::setprecision(2) << std::fixed << "Accuracy: Correctly identified " << correct << " out of " << p_points << " (" << ((double)correct / p_points) * 100 << "%)\n";
				else  std::cout << std::setprecision(2) << std::fixed << "Accuracy: Correctly identified " << correct << " out of " << p_points << " (100.00%)\n";
			}
		}
		else if (!log) std::cout << "Cannot verify accuracy (Was not included in test file)\n";
		else logfile << "Cannot verify accuracy (Was not included in test file)\n";
	}
}

// Gets the name of the testfile, asks for the number of K to use in K-Means, asks if there is a test variable included in file and allows option to reserve 10% of points for test after training on the rest
// Then reads the file of Points according to data given by user. Terminates upon invalid input/test file, and queries user whether to try on a new file or exit program
int get_rdata(int& no_points, int& r_points, Plist*& rhead, Plist*& head, Plist*& nhead, std::string& fname, std::ifstream& input, int rep, Kmeans*& kmeans, RD& rd) {
	rd.reset();
	if (input.is_open()) input.close();
	std::string inp;
	std::cout << "Enter the filename to be try k-means clustering on.\n";
	std::cin.clear();
	std::getline(std::cin, fname);
	input.open(fname);
	if (!input.is_open()) return -1;
	std::size_t sz;
	while (true) {
		std::cout << "How many categories are there? (How many k-means clusters shall we create?)\n";
		std::getline(std::cin, inp);
		if (s_is_n(inp)) {
			rd.K = stoi(inp, &sz);
			break;
		}
		else {
			if (inp[0] == 'q' || inp[0] == 'Q') return -100;
			std::cout << "That was not a number. Type 'Quit' or 'Q' to exit or enter the number of categories to create.\n";
			std::fflush(stdin);
		}
	}
	while (true) {
		std::cout << "Which categorical variable would you like to test k-means clustering on?\nNote that '1' would correspond to the first variable written in the data file.\nAlternatively type 'None' or 'N' if there is no categorical variable located in the file.\n";
		std::cin.clear();
		std::getline(std::cin, inp);
		if (inp[0] == 'N' || inp[0] == 'n') {
			rd.var1 = 0;
			break;
		}
		else if (s_is_n(inp)) {
			rd.var1 = stoi(inp, &sz);
			if (rd.var1 < 1) {
				std::cout << "Choose a variable greater than or equal to 1 (and less than the number of variables in the file)\n";
				continue;
			}
			rd.var1 = stoi(inp, &sz);
			break;
		}
		else {
			if (inp[0] == 'q' || inp[0] == 'Q') return -100;
			std::cout << "That was not a number. Type 'Quit' or 'Q' to exit or enter the variable to be tested.\n";
			std::fflush(stdin);
		}
	}
	bool reserve = false;
	rd.var1 -= 1;
	if (rd.var1 > -1) {
		while (true) {
			std::cout << "Would you like to reserve every tenth entry to be used for testing after creating clusters? (Y/N)\n";
			std::cin.clear();
			std::getline(std::cin, inp);
			if (inp[0] == 'n' || inp[0] == 'N') break;
			else if (inp[0] == 'y' || inp[0] == 'Y') {
				reserve = true;
				break;
			}
		}
	}

	std::cout << "Attempting to read the given data file.\n";
	// Read file
	bool skip = false;
	Plist* rtemp = NULL;
	Plist* curr = NULL;
	Plist* temp = NULL;
	std::string line;
	Point* p;
	while (!input.eof()) {
		std::getline(input, line);
		if((no_points+1)%9 == 0 && !skip) p = Get_point(line, no_points, r_points, 1, kmeans, rd, reserve);
		else p = Get_point(line, no_points, r_points, 1, kmeans, rd, false);
		if (p == NULL) return -3;
		else if (p->get_id() == -1) continue;
		else if (p->get_id() == -2) {
			std::cout << "Found inconsistently formatted data element on line: " << no_points + 1 << ". Kmeans cannot work on inconsistent data.\n";
			return -3;
		}
		else if (p->get_id() == -3) {
			std::cout << "Failed to process data point " << no_points + 1 << ": run terminated\n";
			return -3;
		}
		else if (p->get_id() == -4) {
			std::cout << "Variable #" << rd.var1+1 << " was selected to be tested on, but there are only " << rd.no_fields << " fields in an entry.\n";
			return -3;
		}
		temp = new Plist;
		temp->point = p;
		if (!skip && reserve && (no_points+1) % 9 == 0) {
			if (rhead == NULL) {
				rhead = temp;
				rtemp = temp;
			}
			else {
				rtemp->nxt = temp;
				rtemp = temp;
			}
			skip = true;
			r_points++;
			continue;
		}
		if (no_points == 0) {
			head = temp;
			curr = head;
		}
		else {
			curr->nxt = temp;
			curr = temp;
		}
		if (reserve && skip) skip = false;
		no_points++;
	}
	if (rd.K > no_points) {
		std::cout << "We cannot have more clusters than there are points in the file!\n";
		return -3;
	}
	if (rd.var1 == -1) rd.unique = 1;
	temp = head;
	for (int u = 0; u < no_points; u++) {
		p = temp->point;
		std::vector<double> f = p->get_fields();
		if (!Update_means(p->get_type(), f, u, kmeans, rd, p->g_atarashi())) {
			std::cout << "Failure updating mean of point " << u << "\n";
			return -3;
		}
		temp = temp->nxt;
	}
	if (rd.negz != NULL) {
		Positivize(head, rd, no_points);
	}
	if (reserve) {
		rtemp = rhead;
		while (rtemp != NULL) {
			for (int i = 0; i < rd.unique; i++)
				if (rtemp->point->get_undec() == kmeans->typez.at(i))
					rtemp->point->assign_type(i);
			rtemp = rtemp->nxt;
		}
	}
	double max = 0;
	for (int fmax = 0; fmax < rd.no_fields; fmax++)
		if (max < rd.maxval[fmax]) max = rd.maxval[fmax];
	rd.maxv = max;
	input.close();
	if (no_points < 1) return -4;
	rd.used_heads = new int[no_points];
	for (int xvi = 0; xvi < no_points; xvi++)
		rd.used_heads[xvi] = 0;
	Nominalize(head, nhead, rd, no_points);
	return 1;
}

// Runs collected test data through the K-Means clustering algorithm
// Will attempt to create the nominalized and regular clusters
bool run(Kmeans*& kmeans, Kmeans*& nom_kmeans, int no_points, Plist*& head, Plist*& nhead, RD& rd) {
	kmeans->precision = 0;
	if(!Find_clusters_centers(no_points, head, kmeans, rd)) return false;
	if(nom_kmeans == NULL) {
		nom_kmeans = new Kmeans(rd);
		nom_kmeans->cp(kmeans, rd);
		nom_kmeans->nominalized = true;
		for (int i = 0; i < rd.unique; i++) {
			if(rd.var1 > -1) nom_kmeans->typez.push_back(kmeans->typez.at(i));
			nom_kmeans->categories.push_back(kmeans->categories.at(i));
		}
	}
	nom_kmeans->precision = 0;
	if (!Find_clusters_centers(no_points, nhead, nom_kmeans, rd)) return false;
	return true;
}

// Resets all runtime data- called before running on a new file
void reset_Kmeans(int& p_points, int& r_points, Plist*& rhead, Plist*& phead, Plist*& nhead, std::vector<int>*& prediction, Plist*& head, RD& rd, int& no_points, Kmeans*& kmeans, Kmeans*& nmeans) {
	p_points = 0;
	Plist* rmv = phead;
	while (rmv != NULL) {
		Plist* nxtrmv = rmv->nxt;
		delete rmv;
		rmv = nxtrmv;
	}
	phead = NULL;
	if (prediction != NULL) {
		prediction = new std::vector<int>;
	}
	r_points = 0;
	no_points = 0;
	rmv = head;
	while (rmv != NULL) {
		Plist* nxtrmv = rmv->nxt;
		delete rmv;
		rmv = nxtrmv;
	}
	head = NULL;
	rmv = nhead;
	while (rmv != NULL) {
		Plist* nxtrmv = rmv->nxt;
		delete rmv;
		rmv = nxtrmv;
	}
	nhead = NULL;
	rmv = rhead;
	while (rmv != NULL) {
		Plist* nxtrmv = rmv->nxt;
		delete rmv;
		rmv = nxtrmv;
	}
	rhead = NULL;
	rd.reset();
	delete kmeans;
	kmeans = NULL;
	delete nmeans;
	nmeans = NULL;
}

// Makes predictions on a new file after creating the clusters for k-means
// Can use the nominalzied or regular data set
int predict(int& p_pointz, Plist*& phead, std::string& fname, std::ifstream& input, std::vector<int>*& prediction, int& err, bool& remd, Kmeans* kmeans, RD& rd) {
	remd = true;
	std::fflush(stdin);
	input.close();
	if (p_pointz == 0) {
		while (true) {
			if (rd.var1 > -1) std::cout << "Enter the filename to make predictions on.\n";
			else std::cout << "Enter the filename to make place points.\n";
			std::cin.clear();
			std::getline(std::cin, fname);
			input.open(fname);
			if (input.is_open()) break;
			Log_resp(fname);
			if (fname[0] == 'R' || fname[0] == 'r') return 5;
			else if (fname[0] == 'Q' || fname[0] == 'q') return -1;
			else continue;
		}
		p_pointz = 0;
		phead = NULL;
		Plist* curr = NULL;
		Plist* temp = NULL;
		std::string line;
		Point* p;
		while (!input.eof()) {
			std::getline(input, line);
			if (p_pointz == 0) {
				int elementz = 1;
				for (unsigned i = 0; i < line.length(); i++)
					if (line[(int)i] == ',') elementz++;
				if (elementz == rd.no_fields - 1) {
					remd = false;
				}
				else if (elementz != rd.no_fields) {
					std::cout << elementz << " to " << rd.no_fields << "\n";
					std::cout << "Found inconsistently formatted data element on line: " << p_pointz + 1 << ". cannot work on incomplete/inconsistent data.\n";
					return -3;
				}
			}
			if (remd) p = Get_point(line, p_pointz, 0, -1, kmeans, rd, false);
			else p = Get_point(line, p_pointz, 0, -2, kmeans, rd, false);
			if (p == NULL) return -5;
			else if (p->get_id() == -1) continue;
			else if (p->get_id() == -2) {
				std::cout << "Found inconsistently formatted data element on line: " << p_pointz + 1 << ". Kmeans cannot work on incomplete/inconsistent data.\n";
				return -3;
			}
			temp = new Plist;
			temp->point = p;
			if (p_pointz == 0) {
				phead = temp;
				curr = phead;
			}
			else {
				curr->nxt = temp;
				curr = temp;
			}
			p_pointz++;
		}
	}
	if (kmeans->nominalized) {
		Plist* nom = new Plist;
		if(rd.negz != NULL) Positivize(phead, rd, p_pointz);
		Nominalize(phead, nom, rd, p_pointz);
		phead = nom;
	}
	Identify_clusters(p_pointz, phead, false, prediction, kmeans, rd);
	return 1;
}
