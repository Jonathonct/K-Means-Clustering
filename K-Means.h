/* KMeans.h : Defines the entry point for the console application.
** Completed by Jonathon Teague on 3/28/18
** Implements the class structure used by K-Means
*/

#include <vector>
#include <string>
#include <cctype>
#include <iostream> // Included to log exceptions from default deconstructor/constructor
#include <exception> // Included to log exceptions from default deconstructor/constructor

// Structure to hold various runtime data
struct RD {
	int rt; // # of times to run to find optimal set using different starting points
	double* maxval; // Highest value found within each category
	double* minval; // Minimal value found within each category 
	int no_fields; // Number of data fields detected in file
	int K; // User-entered number of clusters to create
	int var1; // Categorical variable to test for (-1 if none is selected)
	int unique; // Number of unique sub categories of var1
	double maxv; // Maximum value detected in file (used to format)
	int* used_heads; // List of used points for cluster heads
	int last; // Last point used as a cluster head
	double *negz; // Used to find indices of negative values to 'positivize'
	char separator;
	void set_by_fields(std::vector<double> field) {
		maxval = new double[no_fields];
		minval = new double[no_fields];
		for (int xiv = 0; xiv < no_fields; xiv++) {
			maxval[xiv] = field.at(xiv);
			minval[xiv] = field.at(xiv);
		}
	}
	void reset() {
		separator = ',';
		negz = NULL;
		maxval = NULL;
		minval = NULL;
		rt = 0;
		unique = 0;
		no_fields = 0;
		K = -1;
		maxv = 0.0;
	}
};

// Point class: holds the parsed data read from the given data file
class Point {
private:
	int point_id, cluster_no, type_no, no_miss;
	bool *missing;
	std::vector<double> fields;
	std::vector<double>* nom_fields;
	std::string undec;
	std::vector<double> tru_val;
	bool atarashi;
public:
	Point() {
		no_miss = 0;
		missing = NULL;
		type_no = -1;
		cluster_no = -1;
		point_id = -1;
		atarashi = false;
		if (std::uncaught_exception()) { std::cout << "Unhandled exception in creating empty Point\n"; }
	}
	Point(int pointid, std::vector<double> entry, int typeno) {
		no_miss = 0;
		missing = NULL;
		atarashi = false;
		type_no = typeno;
		cluster_no = -1;
		fields = entry;
		nom_fields = NULL;
		point_id = pointid;
		if (std::uncaught_exception()) { std::cout << "Unhandled exception in creating Point\n"; }
	}
	Point(int pointid, std::vector<double> entry, std::vector<double>* nom_entry, int typeno) {
		no_miss = 0;
		missing = NULL;
		atarashi = false;
		type_no = typeno;
		cluster_no = -1;
		fields = entry;
		nom_fields = nom_entry;
		point_id = pointid;
		if (std::uncaught_exception()) { std::cout << "Unhandled exception in creating Point\n"; }
	}
	~Point() {
		if (std::uncaught_exception()) { std::cout << "Uncaught exception in deleting Point\n"; }
	}

	std::vector<double> get_fields() { return fields; }
	std::vector<double> get_tru() { return tru_val; }
	std::vector<double>* get_nom_fields() { return nom_fields; }
	int get_type() { return type_no; }
	int get_id() { return point_id; }
	int get_cluster() { return cluster_no; }
	bool* get_miss() { return missing; }
	bool g_atarashi() { return atarashi; }
	int get_no_miss() { return no_miss; }
	std::string get_undec() { return undec; }
	void adjust(RD rd, int no_points) {
		if (tru_val.empty()) {
			for (int old = 0; old < rd.no_fields; old++)
				tru_val.push_back(fields.at(old));
		}
	}
	void positivize(int wher, double by) { 
		fields.at(wher) = fields.at(wher) + by; 
	}
	void s_miss(bool* m, int no_fields) { 
		missing = new bool[no_fields]; 
		for (int i = 0; i < no_fields; i++) {
			missing[i] = m[i];
			no_miss++;
		}
	}
	void assign_cluster(int c) { cluster_no = c; }
	void set_undec(std::string u) { undec = u; }
	void assign_type(int c) { type_no = c; }
	void s_atarashi() { atarashi = true; }
};

// Linked list of Points- used for predictions and the points drawn from the learning file
struct Plist {
	Point* point = NULL;
	Plist* nxt = NULL;
	~Plist() {
		if (std::uncaught_exception()) { std::cout << "Uncaught exception in deleting Plist\n"; }
	}
	Plist() {
		if (std::uncaught_exception()) { std::cout << "Unhandled exception in creating Plist\n"; }
	}
};

// Cluster class to contain points of similarity found via the K-Means algorithm
class Cluster {
	int cluster_no;
	int no_points;
	int* v;
	int* missing;
	double* share;
	double *meanz, *nom_meanz, *valuez, *nom_valuez;
	std::vector<double>* c_k_meanz;
	std::vector<double>* nom_k_meanz;
	std::vector<int> typed;
	std::vector<Point> points;
public:
	Cluster(int i, Point* p, RD& rd) {
		if (rd.K < 1) {
			return;
		}
		c_k_meanz = new std::vector<double>[rd.no_fields];
		if (!(p->get_nom_fields() == NULL)) {
			nom_k_meanz = new std::vector<double>[rd.no_fields];
			nom_meanz = new double[rd.no_fields];
			nom_valuez = new double[rd.no_fields];
		}
		else {
			nom_k_meanz = NULL;
			nom_meanz = NULL;
			nom_valuez = NULL;
		}
		v = new int[rd.unique];
		for (int i = 0; i < rd.unique; i++) {
			typed.push_back(0);
			v[i] = 0;
		}
		if (p->get_no_miss() > 0) {
			missing = new int[rd.no_fields];
			for (int m = 0; m < rd.no_fields; m++)
				if (p->get_miss()[m]) missing[m] = 1;
				else missing[m] = 0;
		}
		else missing = NULL;
		for (int xP = 0; xP < rd.unique; xP++) {
			for (int Px = 0; Px < rd.no_fields; Px++) {
				if (p->get_type() == xP) {
					c_k_meanz[Px].push_back(p->get_fields().at(Px));
					if (nom_k_meanz != NULL) nom_k_meanz[Px].push_back(p->get_nom_fields()->at(Px));
				}
				else {
					c_k_meanz[Px].push_back(0);
					if (nom_k_meanz != NULL) nom_k_meanz[Px].push_back(0);
				}
			}
			typed.at(p->get_type()) = 1;
			if (p->get_type() > -1) v[p->get_type()]++;
		}
		no_points = 1;
		cluster_no = i;
		if (p != NULL) points.push_back(*p);
		meanz = new double[rd.no_fields];
		valuez = new double[rd.no_fields];
		for (int i = 0; i < rd.no_fields; i++) {
			if (nom_k_meanz != NULL) {
				nom_valuez[i] = p->get_nom_fields()->at(i);
				nom_meanz[i] = nom_valuez[i];
			}
			if(rd.negz == NULL) valuez[i] = p->get_fields().at(i);
			else valuez[i] = p->get_tru().at(i);
			meanz[i] = valuez[i];
		}
		if (std::uncaught_exception()) { std::cout << "Unhandled exception in creating Cluster\n"; }
	}
	~Cluster() {
		if (std::uncaught_exception()) { std::cout << "Unhandled exception in deleting Cluster\n"; }
	}
	std::vector<Point> get_points() { return points; }
	int get_no_points() { return no_points; }
	int get_typed(int i) { return typed.at(i); }
	double get_meanz(int i) { return meanz[i]; }
	double get_nom_meanz(int i) { return nom_meanz[i]; }
	double get_valuez(int i) { return valuez[i]; }
	void add_type(int tnum) { typed[tnum]++; }
	void Add_point(Point& p, RD& rd) {
		no_points++;
		if (p.get_no_miss() > 0) {
			if (missing == NULL) {
				missing = new int[rd.no_fields];
				for (int m = 0; m < rd.no_fields; m++)
					missing[m] = 0;
			}
			for (int m1 = 0; m1 < rd.no_fields; m1++)
				if (p.get_miss()[m1]) missing[m1]++;
		}
		while (typed.size() < rd.unique)
			typed.push_back(0);
		typed.at(p.get_type()) = typed.at(p.get_type()) + 1;
		std::vector<double> newz;
		if (rd.negz == NULL) newz = p.get_fields();
		else newz = p.get_tru();
		std::vector<double>* nom_newz = p.get_nom_fields();
		int thistype = p.get_type();
		for (int i = 0; i < rd.no_fields; i++) {
			if (p.get_no_miss() > 0)
				if (p.get_miss()[i]) continue;
			c_k_meanz[i].at(thistype) = c_k_meanz[i].at(thistype) + newz.at(i);
			if (nom_k_meanz != NULL) {
				nom_k_meanz[i].at(thistype) = nom_k_meanz[i].at(thistype) + nom_newz->at(i);
				nom_valuez[i] += nom_newz->at(i);
				if(missing == NULL) nom_meanz[i] = nom_valuez[i] / no_points;
				else nom_meanz[i] = nom_valuez[i] / (no_points - missing[i]);
			}
			valuez[i] += newz.at(i);
			if (missing == NULL) meanz[i] = valuez[i] / no_points;
			else meanz[i] = valuez[i] / (no_points - missing[i]);
		}
		if (p.get_type() > -1) v[p.get_type()]++;
		p.assign_cluster(cluster_no);
		points.push_back(p);
	}
	void set_share(std::vector<int> categories, RD& rd) {
		while (typed.size() < rd.unique)
			typed.push_back(0);
		share = new double[rd.unique];
		for (int i = 0; i < rd.unique; i++) {
			share[i] = (double)typed[i]/ categories.at(i);
		}
	}
	double get_submean(int ___, int _____) {
		if(nom_k_meanz == NULL) return c_k_meanz[___].at(_____);
		else return nom_k_meanz[___].at(_____);
	}
	void update_meanz(RD& rd) {
		if (c_k_meanz == NULL) {
			std::cout << "Error allocating data.\n";
			return;
		}
		for(int pog = 0; pog < rd.unique; pog++){
			if (typed[pog] == 0) continue;
			for (int champ = 0; champ < rd.no_fields; champ++) {
				c_k_meanz[champ].at(pog) = c_k_meanz[champ].at(pog) / typed[pog];
				if (nom_k_meanz != NULL) nom_k_meanz[champ].at(pog) = nom_k_meanz[champ].at(pog) / typed[pog];
			}
		}
	}
	double get_share(int i) { return share[i]; }
	bool Remove_point(Point& p, RD& rd) {
		typed.at(p.get_type()) = typed.at(p.get_type()) - 1;
		int search = p.get_id();
		int loc = -1;
		for (int i = 0; i < no_points; i++)
			if (points.at(i).get_id() == search)
				loc = i;
		if (loc == -1) {
			return false;
		}
		no_points--;
		if (p.get_no_miss() > 0) {
			for (int m1 = 0; m1 < rd.no_fields; m1++)
				if (p.get_miss()[m1]) missing[m1]--;
		}
		std::vector<double> newz;
		if(rd.negz == NULL) newz = p.get_fields();
		else newz = p.get_tru();
		std::vector<double>* nom_newz = p.get_nom_fields();
		int thistype = p.get_type();
		if (p.get_type() > -1) v[p.get_type()]--;
		for (int i = 0; i < rd.no_fields; i++) {
			if (p.get_no_miss() > 0)
				if (p.get_miss()[i]) continue;
			c_k_meanz[i].at(thistype) = c_k_meanz[i].at(thistype) - newz.at(i);
			if (nom_k_meanz != NULL) {
				nom_k_meanz[i].at(thistype) = nom_k_meanz[i].at(thistype) - nom_newz->at(i);
				nom_valuez[i] -= nom_newz->at(i);
				if (missing == NULL) nom_meanz[i] = nom_valuez[i] / no_points;
				else nom_meanz[i] = nom_valuez[i] / (no_points - missing[i]);
			}
			valuez[i] -= newz.at(i);
			if (missing == NULL) meanz[i] = valuez[i] / no_points;
			else meanz[i] = valuez[i] / (no_points - missing[i]);
		}
		points.erase(points.begin() + loc);
		return true;
	}
};

// Global class to contain the clusters and data shared by the clusters
class Kmeans {
public:
	bool first, nominalized, named;
	Cluster** clusters;
	double* gmeans;
	std::vector<int> categories;
	std::vector<std::string> typez;
	std::vector<std::string> catz;
	double precision;
	std::vector<double> *typ_sum, *typ_means;

	Kmeans(RD& rd) {
		if (rd.K < 1) {
			return;
		}
		named = false;
		nominalized = false;
		first = false;
		typ_means = new std::vector<double>[rd.no_fields];
		typ_sum = new std::vector<double>[rd.no_fields];
		precision = 0;
		clusters = new Cluster*[rd.K];
		gmeans = new double[rd.no_fields];
		for (int i = 0; i < rd.no_fields; i++) {
			if(i < rd.K) clusters[i] = NULL;
			gmeans[i] = 0;
		}
		if (std::uncaught_exception()) { std::cout << "Unhandled exception in creating kmeans class\n"; }
	}
	~Kmeans() {
		if (std::uncaught_exception()) { std::cout << "Uncaught exception in deleting Kmeans\n"; }
	}

	void cp(Kmeans* k, RD& rd) {
		named = k->named;
		first = false;
		for (int x = 0; x < k->catz.size(); x++)
			catz.push_back(k->catz.at(x));
		if(rd.var1 > -1)
			for (int i = 0; i < rd.unique; i++) {
				typez.push_back(k->typez.at(i));
				catz.push_back(k->typez.at(i));
				categories.push_back(k->categories.at(i));
				for (int x = 0; x < rd.no_fields; x++) {
					if (i == 0) gmeans[x] = k->gmeans[x];
					typ_means[x].push_back(k->typ_means[x].at(i));
					typ_sum[x].push_back(k->typ_sum[x].at(i));
				}
			}
	}

	void update_clusters(Plist* head, int no_points) {
		Plist* temp = head;
		int typ;
		for (int i = 0; i < no_points; i++) {
			if (temp->point == NULL) break;
			typ = temp->point->get_type();
			clusters[temp->point->get_cluster()]->add_type(typ);
			if (temp->nxt == NULL) break;
			temp = temp->nxt;

		}
	}
};

// Misc function declarations implemented in K-Means.cpp:
double Euclid(Point* p, int x, bool remd, int test_no, Kmeans*& kmeans, RD& rd);
int Find_match(Point* p, bool remd, Kmeans*& kmeans, RD& rd);
double Adjust_prob(Point* p, int wher, Kmeans*& kmeans, RD& rd);
bool Identify_clusters(int no_pointz, Plist*& head, bool remd, std::vector<int>*& prediction, Kmeans*& kmeans, RD& rd);
bool Find_clusters_centers(int no_points, Plist*& head, Kmeans*& kmeans, RD& rd);
int* Find_heads(int no_points, Plist*& head, Kmeans*& kmeans, RD& rd, int seed);
void Assign_rest(int no_points, Plist*& head, Kmeans*& kmeans, RD& rd, int*& indexes);
void Do_Kmeans(int no_points, Plist*& head, Kmeans*& kmeans, RD& rd);
void Positivize(Plist*& head, RD& rd, int no_points);
void Nominalize(Plist*& head, Plist*& nhead, RD& rd, int no_points);
bool Update_means(int _type, std::vector<double> entry, int no_points, Kmeans*& kmeans, RD& rd, bool atarashi);
bool s_is_n(std::string s);
Point* Get_point(std::string& line, int& no_points, int r_points, int rev, Kmeans*& kmeans, RD& rd, bool reserve);
void Finalize_means(int no_points, Kmeans*& kmeans, RD& rd);