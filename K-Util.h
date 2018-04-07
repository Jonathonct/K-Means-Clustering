/* KmeansUtil.cpp : Defines the entry point for the console application.
** Completed by Jonathon Teague on 3/28/18
** Header file for K-Util.cpp
*/

void show_spread(int no_points, Kmeans*& kmeans, RD& rd, std::ofstream& logfile, bool log, std::string fname);
std::string disp_res(int no_points, Kmeans*& kmeans, RD& rd, std::ofstream& logfile, bool log, bool test, std::string inp);
void disp_predictions(std::vector<int> predictions, int p_points, Plist*& head, bool remd, Kmeans*& kmeans, bool part, bool log, std::ofstream& logfile, RD& rd);
void reset_Kmeans(int& p_points, int& r_points, Plist*& rhead, Plist*& phead, Plist*& nhead, std::vector<int>*& prediction, Plist*& head, RD& rd, int& no_points, Kmeans*& kmeans, Kmeans*& nmeans);
int predict(int& p_pointz, Plist*& phead, std::string& fname, std::ifstream& input, std::vector<int>*& prediction, int& err, bool& remd, Kmeans* kmeans, RD& rd);
bool run(Kmeans*& kmeans, Kmeans*& nom_kmeans, int no_points, Plist*& head, Plist*& nhead, RD& rd);
int get_rdata(int& no_points, int& r_points, Plist*& rhead, Plist*& head, Plist*& nhead, std::string& fname, std::ifstream& input, int rep, Kmeans*& kmeans, RD& rd);
int Log_f(int no_points, int p_points, Kmeans*& kmeans, Kmeans*& nom_kmeans, RD& rd, Plist*& phead, bool part, bool remd, bool test, bool nom, std::string& ifname, std::string& pfname, std::vector<int>*& predicitons);