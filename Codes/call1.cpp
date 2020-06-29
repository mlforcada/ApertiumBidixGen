#include "Graph.cpp"
#include "Biconnected.cpp"
#include "DensityAlgo.cpp"
#include<set>
#include<iostream>
#include<chrono>

using namespace std::chrono;

//Used to time code
struct Stopwatch{
    decltype(high_resolution_clock::now()) sTime;
    decltype(high_resolution_clock::now()) eTime;
    inline void start(){
        sTime = high_resolution_clock::now();
    }
    inline void end(){
        eTime = high_resolution_clock::now();
    }
    long long report(){
        return duration_cast<microseconds>(eTime - sTime).count();
    }
    void log(string s=""){
        cout<<"Time taken "<<s<<" : "<<report()/1e6<<" seconds"<<endl;
    }
};

//get statistics on the biconnected components
void getStatsComps(Graph &G)
{
    Biconnected B;
    B.findBicomps(G);
    ofstream compfile;
    compfile.open("../Main/Analysis/TDbigcompfile-eng.txt");
    int subgraphnum = 0;
    int mxv = 0, mxe = 0, cntg100=0, cntg1000=0; //max size and count of comps with >100 or >1000 vertices
    for(auto SG: G.subGraphs)
    {
        if(SG.vertices.size()>=100) cntg100++;
        if(SG.vertices.size()>=1000) cntg1000++;
        if(SG.vertices.size()<=2) continue;
        compfile << "Num vertices: " << SG.vertices.size() << endl;
        compfile << "Num edges: " << SG.num_edges << endl;
        mxv = max(mxv, (int) SG.vertices.size());
        mxe = max(mxe, (int) SG.num_edges);
        subgraphnum++;
        compfile << subgraphnum << endl;
        set<string> engwords;
        for(auto u: SG.vertices) //print only english words
        {
            if(u.rep.lang=="en") engwords.insert(u.rep.surface);
        }
        for(auto u: engwords)
        {
            compfile << u << endl;
        }
        compfile << endl;
    }
    cout << mxv << " " << mxe << endl;
    cout << cntg100 << " " << cntg1000 << endl;
    compfile.close();
}

//load the pairs listed in LangData-List into the graph
void runPairs(Graph &G, int idxign)
{
    int num_pairs = 11; //number of pairs of languages.
    string input_file;
    ifstream file_list;
    file_list.open("../Main/LangData-List.txt"); //file with names of lang pairs
    ofstream fout;
    fout.open("../Main/Analysis/Tempfile.txt");
    int prev_nodes = 0, prev_edges = 0; //cumalative vertices/edges till now
    for(int i = 0; i < num_pairs; i++)
    {
        file_list >> input_file;
        if(i==idxign) continue;
        cout << input_file << endl;
        fout << input_file << endl;
        G.loadData(input_file, fout);
        fout << "Number of vertices: " << G.vertices.size() - prev_nodes << endl; //vertices in this file
        fout << "Number of edges: " << G.num_edges - prev_edges << endl; //edges in this file
        prev_nodes = G.vertices.size(); prev_edges = G.num_edges;
        cout << "done" << endl;
    }
}

string l1, l2;
void predByLang(string &file_pref, map<string, Graph> &pred){
    ofstream summary; summary.open(file_pref + "-summary.txt");
    for(auto &langpair: pred){
        if(langpair.first!=l1 && langpair.first!=l2) continue;
        Graph &langG = langpair.second;
        summary << langpair.first << endl;
        summary << "Number of vertices: " << langG.vertices.size() << endl;
        summary << "Number of edges: " << langG.num_edges << endl;
        string file_name = file_pref + "_" + langpair.first + ".txt";
        ofstream outfile; outfile.open(file_name);
        langG.printGraph(outfile);
        summary << endl; //blank line
    }
}

//Load a small word based context graph
void runWords(Graph &G, string &word)
{
    string fin_name = "../Main/SampleWord/" + word + ".txt";
    ofstream fout;
    fout.open("../Main/Results/" + word + "_analysis.txt");
    G.loadData(fin_name, fout); //unidirectional data load
    cout << "loaded" << endl;
}

//Run after precomputing biconnected components
int runBicomp(Graph &G, Config &config, string &prefix, map<string, Graph> &pred)
{
    string fileout_name = "../Main/Results/" + prefix + "bicomp_out.txt";
    ofstream fout;
    fout.open(fileout_name); fout.close();
    int new_trans=0;
    Biconnected B; //object of biconnected computation class
    B.findBicomps(G);
    for(auto SG: G.subGraphs) //iterate over components and run density algo for each
    {
        DensityAlgo D = DensityAlgo(SG, config);
        new_trans += D.run(fileout_name, pred); //append output to fileout_name
    }
    string pred_file_name = "../Main/Results/RemLang/" + prefix;
    predByLang(pred_file_name, pred);
    return new_trans;
}

//Run directly without precomputing biconnected components
int runDirect(Graph &G, Config &config, string &prefix, map<string, Graph> &pred)
{
    string fileout_name = "../Main/Results/" + prefix + "_out.txt";
    ofstream fout;
    fout.open(fileout_name); fout.close();
    DensityAlgo D(G, config);
    int new_trans = D.run(fileout_name, pred);
    string pred_file_name = "../Main/Results/" + prefix;
    predByLang(pred_file_name, pred);
    return new_trans;
}

int main()
{
    Stopwatch timer;
    l1 = "oc-ca"; l2="ca-oc";
    string word = "rem_" + l1;
    //cin >> word;
    Graph G;
    runPairs(G, 8);
    //getStatsComps(G);
    //runWords(G, word);
    /*ofstream debugfile;
    //debugfile.open("../Main/Results/debugG.txt");
    G.printGraph(debugfile);*/
    Config config;
    timer.start();
    map<string, Graph> predicted; //string stores language pair and maps it to a graph
    int new_trans = runBicomp(G, config, word, predicted);
    cout << new_trans << endl;
    timer.end();
    timer.log();
}
