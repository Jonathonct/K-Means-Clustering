/* KMeans.cpp : Defines the entry point for the console application.
** Completed by Jonathon Teague on 3/28/18
** Implements the functions for finding clusters, updating clusters, testing for matches, etc
*/

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdlib.h>
#include <cstdio>
#include <vector>
#include <time.h>
#include <math.h>
#include "K-Means.h"

// Sum of Euclid's formula for each data section: http://www.cplusplus.com/forum/beginner/178293/
double Euclid(Point* p, int x, bool remd, int test_no, Kmeans*& kmeans, RD& rd) {
	double sum = 0.0;
	int remdi = 0;
	std::vector<double> xD, *Dx;
	Dx = NULL;
	if(!kmeans->nominalized) xD = p->get_fields();
	else Dx = p->get_nom_fields();
	bool* mia = p->get_miss();
	for (int i = 0; i < rd.no_fields; i++) {
		if (remd == true && i == rd.var1) {
			continue;
		}
		else if (mia != NULL) {
			if (mia[i]) continue;
		}
		else if (remd == true) {
			if (!kmeans->nominalized) sum += pow(kmeans->clusters[x]->get_meanz(i) - xD.at(remdi), 2);
			else sum += pow(kmeans->clusters[x]->get_nom_meanz(i) - Dx->at(remdi), 2);
			remdi++;
		}
		else {
			if (!kmeans->nominalized)sum += pow(kmeans->clusters[x]->get_meanz(i) - xD.at(i), 2);
			else {
				sum += pow(kmeans->clusters[x]->get_nom_meanz(i) - Dx->at(i), 2);
			}
		}
	}
	return sqrt(sum);
}

// Given a Point (attached to a Plist*), find the best matching Cluster by calling Euclid
int Find_match(Point* p, bool remd, Kmeans*& kmeans, RD& rd) {
	int c = 0;
	double close = 0; double tmp = 0;

	for (int x = 0; x < rd.K; x++) {
		tmp = Euclid(p, x, remd, -1, kmeans, rd);	
		if (x == 0) close = tmp;
		else if (tmp < close) {
			close = tmp;
			c = x;
		}
	}
	return c;
}

// Calculates average distance from point p from the centroid of cluster located at wher
double Adjust_prob(Point* p, int wher, Kmeans*& kmeans, RD& rd) {
	double prob = 0;
	int skip = 0;
	for (int i = 0; i < rd.no_fields; i++) {
		if (i == rd.var1) continue;
		else if (p->get_no_miss() > 0)
			if (p->get_miss()[i]) continue;
		if ((abs(rd.maxval[i] - rd.minval[i]) != 0)) {
			if (!(kmeans->nominalized)) prob += abs(kmeans->typ_means[i].at(wher) - p->get_fields().at(i)) / (abs(rd.maxval[i] - rd.minval[i]));
			else prob += abs(kmeans->typ_means[i].at(wher) - p->get_fields().at(i)) / (abs(rd.maxval[i] - rd.minval[i]));
		}
		else skip++;
	}
	prob = (prob / (rd.no_fields - skip - p->get_no_miss()));
	return abs(1.0 - prob);
}

// Identifies a Point's cluster without updating the cluster
// Used to make predictions based on learning set
bool Identify_clusters(int no_pointz, Plist*& head, bool remd, std::vector<int>*& prediction, Kmeans*& kmeans, RD& rd) {
	Plist* temp = head;
	Point* p = NULL;
	int choice = -1;
	double* guess;
	double _K_ = 0.0;
	double __ = 0.0;
	double prob = 0;
	for (int i = 0; i < no_pointz; i++) {
		if (temp->point == NULL) return false;
		guess = new double[rd.unique];
		prob = 0;
		if (i != 0) temp = temp->nxt;
		int c = Find_match(temp->point, remd, kmeans, rd);
		temp->point->assign_cluster(c);
		for (int gogo = 0; gogo < rd.unique; gogo++) {
			if (kmeans->clusters[c]->get_typed(gogo) == 0) {
				guess[gogo] = 0.0;  continue;
			}
			prob = ((double)kmeans->clusters[temp->point->get_cluster()]->get_typed(gogo));
			prob = ((double)prob / kmeans->categories.at(gogo)) * 100;
			guess[gogo] = prob;
		}
		prob = 0.0;
		for (int donezo = 0; donezo < rd.unique; donezo++) {
			if (guess[donezo] == 0) continue;
			if (prob < guess[donezo]) {
				p = temp->point;
				choice = donezo;
				prob = guess[donezo];
			}
			else if(guess[donezo] == prob) {
				double old = prob * Adjust_prob(p, choice, kmeans, rd);
				double n = guess[donezo] * Adjust_prob(temp->point, donezo, kmeans, rd);
				if (n > old) {
					p = temp->point;
					choice = donezo;
					prob = n;
				}
			}
		}
		if (choice > -1) {
			prediction->push_back(choice);
		}
		else prediction->push_back(0);
	}
	return true;
}

/*
  ** First finds K (user-given number of clusters) based on different randomly selected data points
  ** Then assigns every variable to the nearest point to it using Euclid's formula, simultaneously updating the cluster's mean
  ** Then cycles through all the data points reassigning them to the best matching cluster, updating the cluster's mean if so
  ** Will finish when all points have been cycled through without change or if for some reason the operation takes O(x^2) time
 */
bool Find_clusters_centers(int no_points, Plist*& head, Kmeans*& kmeans, RD& rd) {
	if (rd.K > no_points) {
		std::cout << "Cannot have a K value greater than the number of entries.\n";
		return false;
	}
	else if (rd.var1 > rd.no_fields) {
		std::cout << "There are only " << rd.no_fields << " in the data file.\n";
		return false;
	}
	rd.rt = no_points / rd.K;
	if (rd.var1 < 0) rd.rt = 1;
	if (no_points > 2000 && rd.rt > 5) rd.rt = 5;
	else if (rd.rt > 10) rd.rt = 10;
	kmeans->first = true;
	Kmeans* tmeans = kmeans;
	rd.used_heads = new int[no_points];
	for (int r = 0; r < no_points; r++)
		rd.used_heads[r] = 0;
	for (int skt = 0; skt < rd.rt; skt++) {
		if (skt > 0) {
			tmeans = new Kmeans(rd);
			if(kmeans->nominalized) tmeans->nominalized = true;
			tmeans->cp(kmeans, rd);
			tmeans->first = true;
		}
		if(skt == 0 && kmeans->nominalized != true) {std::cout << "\r0.00% complete.."; fflush(stdout);}
		int* indexes = Find_heads(no_points, head, tmeans, rd, skt);
		if (indexes == NULL) return false;
		Assign_rest(no_points, head, tmeans, rd, indexes);
		Do_Kmeans(no_points, head, tmeans, rd);
		Finalize_means(no_points, tmeans, rd);
		for (int i = 0; i < rd.K; i++) {
			tmeans->clusters[i]->set_share(tmeans->categories, rd);
		}
		double p = 0;
		if (rd.var1 > -1) {
			for (int v = 0; v < rd.K; v++) {
				double c_v = 0;
				double c_e = 0;
				for (int w = 0; w < rd.unique; w++)
					if (c_v < tmeans->clusters[v]->get_share(w)) 
						c_v = tmeans->clusters[v]->get_share(w);
					else 
						c_e += tmeans->clusters[v]->get_share(w);
				c_v = c_v - c_e;
				if (c_v < 0) c_v = 0;
				p += c_v;
			}
			p = p * 100;
			p = p / rd.K;
			tmeans->precision = p;
		}
		double done = ((double)skt + 1) / rd.rt * 50;
		std::cout << "\r";
		if (tmeans->nominalized) std::cout << std::fixed << std::setprecision(2) <<  done + 50 << "% complete..";
		else if(done >= 10) std::cout << std::fixed << std::setprecision(2) << done << "% complete..";
		else std::cout << std::fixed << std::setprecision(2) << done << " % complete..";
		if(kmeans->nominalized && done +50 ==100) std::cout << "\n";
		fflush(stdout);
		if (tmeans->precision > kmeans->precision) {
			delete kmeans;
			kmeans = tmeans;
		}
	}
	return true;
}

// Picks K unique cluster centers and returns the indexes which were used (to avoid re-using them in assignment stage)
int* Find_heads(int no_points, Plist*& head, Kmeans*& kmeans, RD& rd, int seed) {
	int* indexes = new int[rd.K];
	int k;
	if(seed == 1) srand(time(NULL));
	int c = no_points / rd.K;
	for (int i = 0; i < rd.K; i++) {
		int tries = 0;
		if (seed == 1) k = rand() % no_points;
		else if (seed % 2 == 0) {
		if(i ==0) k = 0;
		else k =  i * c + seed * 3; 
		}
		else if (seed % 3 == 0) {
			if (i == 0) k = no_points / 2;
			else if (i % 2 == 0) k = rd.last + rd.rt;
			else k = rd.last + rd.K * i * seed;
		}
		else {
			if (i % 3 == 0) k = seed * rd.K * (i+1);
			else if (i % 2 == 0) k = rd.last + 100;
			else k = seed * 10;
		}
		while (k >= no_points) k = k - c;
		while (rd.used_heads[k] == 1) {
			tries++;
			k = k + 1;
			if (k >= no_points) k = k - no_points;
			if (tries == no_points) {
				std::cout << "Failed to create new cluster. Error not handled.\n";
				return NULL;
			}
		}
		indexes[i] = k;
		rd.last = k;
		rd.used_heads[k] = 1;
		Plist* temp = head;
		for (int m = 0; m < k; m++) {
			if (temp != NULL) temp = temp->nxt;
			else {
				std::cout << "Failure to create new cluster. Error not handled.\n";
				return NULL;
			}
		}
		Cluster* clemp;
		if (temp->point == NULL) continue;
		clemp = new Cluster(i, temp->point, rd);
		if (!clemp) {
			std::cout << "Failed to create new cluster. Error not handled.\n";
			return NULL;
		}
		temp->point->assign_cluster(i);
		kmeans->clusters[i] = clemp;
	}
	return indexes;
}

// Assigns every point in the training set to the cluster it most closely matches and updates the cluster's means with its own averaged in
void Assign_rest(int no_points, Plist*& head, Kmeans*& kmeans, RD& rd, int*& indexes) {
	Plist* temp = head;
	int c;
	bool used;
	for (int i = 0; i < no_points; i++) {
		if (temp->nxt == NULL) break;
		else if (i != 0) temp = temp->nxt;
		used = false;
		for (int k = 0; k < rd.K; k++)
			if (indexes[k] == i)
				used = true;
		if (used) continue;
		if (temp->point == NULL)  break;
		if (kmeans->first) {
			c = Find_match(temp->point, false, kmeans, rd);
		}
		else c = Find_match(temp->point, true, kmeans, rd);
		kmeans->clusters[c]->Add_point(*temp->point, rd);
		temp->point->assign_cluster(c);
	}
}

// Now calculate everything using Euclidean Distance and update clusters until no point needs to change
void Do_Kmeans(int no_points, Plist*& head, Kmeans*& kmeans, RD& rd){
	Plist* temp = head;
	int finish = 0;
	int tries = 0;
	int bail = pow(no_points, 1.5); // O(n^3/2) will bail: should pretty much never happen anyway
	int c;
	while (finish < no_points) {
		tries++;
		finish++;
		if (temp->point == NULL) 	temp = head;
		c = Find_match(temp->point, false, kmeans, rd); 
		if (temp->point->get_cluster() != c) {
			kmeans->clusters[temp->point->get_cluster()]->Remove_point(*temp->point, rd);
			temp->point->assign_cluster(c);
			kmeans->clusters[c]->Add_point(*temp->point, rd);
			finish = 0;
		}
		if (tries == bail) break;
		if(temp->nxt != NULL) temp= temp->nxt;
		else temp = head;
	}
}

// Converts all negative entries into positive entries
void Positivize(Plist*& head, RD& rd, int no_points) {
	for (int pos = 0; pos < rd.no_fields; pos++) {
		Plist* nay = head;
		if (rd.negz[pos] < 0) 
			for (int ntop = 0; ntop < no_points; ntop++) {
				if (nay->point == NULL) break;
				nay->point->positivize(pos, abs(rd.negz[pos]));
				nay->point->adjust(rd, no_points);
				nay = nay->nxt;
			}
	}
}

// Converts all numbers to a decimal between 0.0 and 1.0
void Nominalize(Plist*& head, Plist*& nhead, RD& rd, int no_points) {
	double* key = new double[rd.no_fields];
	for (int v = 0; v < rd.no_fields; v++) {
		key[v] = std::abs(rd.maxval[v] - rd.minval[v]);
	}
	Plist* conv = head;
	Plist* temp = NULL;
	Plist* chng = NULL;
	for (int i = 0; i < no_points; i++) {
		std::vector<double>* elementz = new std::vector<double>;
		std::vector<double> pre_elementz;
		std::vector<double> old;
		if(rd.negz == NULL) old = conv->point->get_fields();
		else old = conv->point->get_tru();
		for (int vi = 0; vi < rd.no_fields; vi++) {
			if (vi == rd.var1) {
				pre_elementz.push_back(0);
				elementz->push_back(0);
			}
			else {
				pre_elementz.push_back(old.at(vi));
				elementz->push_back(old.at(vi) / key[vi]);
			}
		}
		Point* p = new Point(conv->point->get_id(), pre_elementz, elementz, conv->point->get_type());
		p->adjust(rd, no_points);
		temp = new Plist;
		temp->point = p;
		if (i == 0) {
			nhead = temp;
			chng = temp;
		}
		else {
			chng->nxt = temp;
			chng = temp;
		}
		if (conv->nxt == NULL) break;
		else conv = conv->nxt;
	}
}

// Sums global means and means for identified type within each cluster
bool Update_means(int _type, std::vector<double> entry, int no_points, Kmeans*& kmeans, RD& rd, bool atarashi) {
	int i = 0;
	if (no_points == 0) { // First entry 
		kmeans->gmeans = new double[rd.no_fields];
		for (i = 0; i < rd.no_fields; i++) {
			kmeans->categories.push_back(1);
			kmeans->gmeans[i] = entry.at(i);
			if(rd.var1 > -1) kmeans->typ_sum[i].push_back(entry.at(i));
		}
	}
	else if (atarashi) { // First entry of new classification type
		for (i = 0; i < rd.no_fields; i++) {
			kmeans->categories.push_back(1);
			kmeans->gmeans[i] = kmeans->gmeans[i] + entry.at(i);
			if (rd.var1 > -1) kmeans->typ_sum[i].push_back(entry.at(i));
		}
	}
	else for (i = 0; i < rd.no_fields; i++) { // Base case
		if (rd.var1 > -1)
			if ((kmeans->typ_sum[_type].size() < 1)) {
				std::cout << "Failed to compute for sum of type '" << kmeans->typez[_type] << "'.\n";
				return false;
			}
		if(i == 0) kmeans->categories.at(_type) = kmeans->categories.at(_type) + 1;
		kmeans->gmeans[i] = kmeans->gmeans[i] + entry.at(i);
		if (rd.var1 > -1) kmeans->typ_sum[i].at(_type) = (kmeans->typ_sum[i].at(_type) + entry.at(i));
	}
	return true;
}

// Finalizes the global means of the kmeans structure
void Finalize_means(int no_points, Kmeans*& kmeans, RD& rd) {
	if(kmeans->first) kmeans->typ_means = new std::vector<double>[rd.no_fields];
	for (int v = 0; v < rd.K; v++) {
		kmeans->clusters[v]->update_meanz(rd);
	}
	for(int w = 0; w < rd.unique; w++)
		if(kmeans->first) for (int poggers = 0; poggers < rd.no_fields; poggers++) {
			kmeans->typ_means[poggers].push_back(0.0);
		}
	if (!kmeans->first) return;
	for (int k = 0; k < rd.no_fields; k++)
		for (int i = 0; i < rd.unique; i++) {
			if(i == 0) kmeans->gmeans[k] = kmeans->gmeans[k] / no_points;
			if (rd.var1 > -1) kmeans->typ_means[k].at(i) = kmeans->typ_sum[k].at(i) / kmeans->categories.at(i);
		}
}

// Returns true if a string represents a number
bool s_is_n(std::string s) {
	int num = 0;
	for (unsigned i = 0; i < s.length(); i++) {
		if (!std::isdigit(s[(int)i]))
			if (s[i] != '.' && s[i] != '-') {
				return false;
			}
		if (num == 0) num = 1;
	}
	if (num == 0) return false;
	return true;
}

// Parses a given line from the CSV file into a new point then adds it to the vector of Points
// Returns a dummy point under various error conditions
Point* Get_point(std::string& line, int& no_points, int r_points, int rev, Kmeans*& kmeans, RD& rd, bool reserve) {
	unsigned i;
	// Determine the number of fields by taking advantage of CSV format & initialize some variables
	if (rev > -1 && rd.no_fields == 0) {
		rd.no_fields = 0;
		for (i = 0; i < line.length(); i++)
			if (line[(int)i] == ',') rd.no_fields++;
		if(rd.no_fields != 0) rd.no_fields++;
	}
	if (rd.no_fields == 0) {
		std::cout << "This program expects comma-separated value (CSV) format by default.\nIf your data elements are separated by a different character, enter it here or enter 'Quit' to try a new set.\n";
		std::string inp;
		std::cin.clear();
		std::getline(std::cin, inp);
		if(inp[0] == 'q' || inp[0] == 'Q') return NULL;
		else {
			char c = inp[0];
			if (rev > -1 && rd.no_fields == 0) {
				for (i = 0; i < line.length(); i++)
					if (line[(int)i] == c) rd.no_fields++;
				rd.no_fields++;
			}
			if (rd.no_fields > 0) rd.separator = c;
			else {
				std::cout << "Failed to acquire new data format.\n";
				return NULL;
			}
		}
	}
	if (kmeans == NULL) {
		kmeans = new Kmeans(rd);
		kmeans->first = true;
	}
	if (rev == -2) rd.no_fields--;
	if (line.empty()) { // empty line
		std::vector<double> dupe;
		Point* p = new Point(-1, dupe, -1);
		return p;
	}

	if (rd.var1 >= rd.no_fields) {
		std::vector<double> dupe;
		Point* p = new Point(-4, dupe, -1);
		return p;
	}

	char nxt = rd.separator;
	std::string conv = "";
	if (no_points == 0 && kmeans->first && kmeans->named == false) {
		int chk = 0;
		std::vector<std::string> cats;
		for (unsigned iv = 0; iv < line.length(); iv++) {
			nxt = line[(int)iv];
			if(nxt == ' ' && rd.separator != ' ') continue;
			else if(nxt == '\t' && rd.separator != '\t') continue;
			if (nxt == rd.separator || iv + 1 == line.length()) {
				if (iv + 1 == line.length() && nxt != '\r') conv += nxt;
				if (!s_is_n(conv)) {
					chk++;
				}
				if (conv.length() > 14)
					conv = conv.substr(0, 14);
				else while (conv.length() < 14)
					conv += " ";
				cats.push_back(conv);
				conv = "";
			}
			if (nxt == rd.separator) conv = "";
			else conv += nxt;
		}
		if (chk > 2 && cats.size() == rd.no_fields) {
			for (int xv = 0; xv < cats.size(); xv++)
				kmeans->catz.push_back(cats.at(xv));
			kmeans->named = true;
			std::vector<double> dupe;
			Point* p = new Point(-1, dupe, -1);
			return p;
		}
		else kmeans->named = false;
	}

	// Extract double values from string of input for the vector for the point
	int iter = 0;
	int ptype = -1;
	bool* mia = NULL;
	std::string type;
	double field;
	std::vector<double> fields;
	conv = "";
	for (unsigned ix = 0; ix < line.length(); ix++) {
		nxt = line[(int)ix];
		if (nxt == ' ' && rd.separator != ' ') continue;
		else if(nxt == '\t' && rd.separator != '\t') continue;
		if (ix + 1 == line.length() || nxt == rd.separator) {
			if (ix + 1 == line.length() && nxt != ' ' && nxt != '\r') conv += nxt;
			if (iter == rd.var1 && rev == -2) {
				fields.push_back((double)0);
				if (!s_is_n(conv)) {
					std::cout << "Failed to convert entry: '" << conv << "' into numerical form.\n";
					std::vector<double> dupe;
					Point* p = new Point(-2, dupe, -1);
					return p;
				}
				if (conv[0] == '.') field = stod("0" + conv);
				fields.push_back(field);
				iter++;
			}
			else if (iter == rd.var1) {
				type = conv;
				iter++;
				fields.push_back((double)0);
			}
			else if (conv[0] == '?') { // Missing field
				if (mia == NULL) {
					mia = new bool[rd.no_fields];
					for (int xi = 0; xi < rd.no_fields; xi++)
						mia[xi] = false;
				}
				fields.push_back(0.0);
				mia[iter] = true;
				iter++;
			}
			else {
				if (!s_is_n(conv)) {
					std::cout << "Failed to convert entry:'" << conv << "' into numerical form\n";
					std::vector<double> dupe;
					Point* p = new Point(-2, dupe, -1);
					return p;
				}
				if (conv[0] == '.') field = stod("0" + conv);
				else field = stod(conv);
				if (field < 0.0) {
					if (rd.negz == NULL) {
						rd.negz = new double[rd.no_fields];
						for (int ne = 0; ne < rd.no_fields; ne++) rd.negz[ne] = 0;
					}
					if (field < rd.negz[iter]) rd.negz[iter] = field;
				}
				fields.push_back(field);
				iter++;
			}
		}
		if (nxt == rd.separator) conv = "";
		else conv += nxt;
	}
	if ((iter != rd.no_fields) || (fields.size() > rd.no_fields + 1) || (fields.size() < rd.no_fields)) { // Inconsistent data- terminal to Kmeans clustering
		std::vector<double> dupe;
		Point* p = new Point(-2, dupe, -1);
		return p;
	}
	bool atarashi = false;
	if (!reserve) {
		if (rd.minval == NULL) {
			rd.set_by_fields(fields);
		}
		else for (int t_t = 0; t_t < rd.no_fields; t_t++) {
			if (rd.maxval[t_t] < fields.at(t_t)) rd.maxval[t_t] = fields.at(t_t);
			else if (rd.minval[t_t] > fields.at(t_t)) rd.minval[t_t] = fields.at(t_t);
		}
		if (rev > -2 && rd.var1 > -1) {
			for (i = 0; i < rd.unique; i++) {
				if (type == kmeans->typez.at(i)) {
					ptype = i;
					break;
					if (rev == -1) {
						std::vector<double> dupe;
						Point* p = new Point(-2, dupe, -1);
						return p;
					}
				}
			}
			if (ptype == -1) {
				kmeans->typez.push_back(type);
				rd.unique++;
				ptype = i;
				atarashi = true;
			}
		}
	}
	if (rd.var1 == -1) ptype = 0;
	Point* p;
	if(r_points == 0) p = new Point(no_points, fields, ptype);
	else {
		p = new Point((no_points + r_points), fields, ptype);
	}
	if (mia != NULL) p->s_miss(mia, rd.no_fields);
	if (reserve) {
		p->set_undec(type);
		return p;
	}
	else if (atarashi) p->s_atarashi();
	if (rev == -2) rd.no_fields++;
	return p;
}
