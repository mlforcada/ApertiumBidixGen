#include "Graph.cpp"
#include "Biconnected.cpp"
#include<set>
#include<iostream>

Graph GO, GC, GE[4], GM[4];
ofstream fout;
map<string, int> OV, OE, CV, CE, EV[4], EE[4], MV[4], ME[4];

//Count by part of speech
void countpos(Graph &G, map<string, int> &POSV, map<string, int> &POSE, ofstream &fout, string title)
{
    for(auto &u: G.vertices){ //Iterate over vertices (words) in the graph
        POSV[u.rep.pos]++; //Increase number of vertices
        POSE[u.rep.pos]+=u.adj.size(); //Increase number of edges
    };
}
//Output the analysis in readable format
void OutReadable(string lang)
{
    string out_name = "../Main/Results/RemLang/Analysis/" + lang + "/POS.txt";
    fout.open(out_name);
    for(auto pos: OE){ //iterate over POS, value pairs in graph of Original edges

        fout << endl << pos.first << endl;
        fout << "Vertices in Original/Correct: " << OV[pos.first] <<  " / " << CV[pos.first] << " (" << CV[pos.first]*100.00/OV[pos.first] << "%)\n";
        fout << "Extra Vertices: (None, " + lang + ", Both)" << endl;
        for(int i = 0; i < 4; i++){
            fout << EV[i][pos.first] << " ";
        }
        fout << endl;
        fout << "Missed Vertices: (None, " + lang + ", Both)" << endl;
        for(int i = 0; i < 4; i++){
            fout << MV[i][pos.first] << " ";
        }
        fout << endl;
        fout << "Edges in Original/Correct: " << OE[pos.first] <<  " / " << CE[pos.first] << " (" << CE[pos.first]*100.00/OE[pos.first] << "%)\n";

        int err = 0, miss = 0;
        fout << "Extra Edges (in P, not in O, classified by O): (None, " + lang + ", Both)" << endl; //Extra
        for(int i = 0; i < 4; i++){
            fout << EE[i][pos.first] << " (" << (double) EE[i][pos.first] * 100.00 / OE[pos.first] << "%)" << "; ";
            err += EE[i][pos.first];
        }
        fout << endl;
        fout << "Missed Edges (in O, not in P, classified by D): (None, " + lang + ", Both)" << endl; //Missed
        for(int i = 0; i < 4; i++){
            fout << ME[i][pos.first] << " (" << (double) ME[i][pos.first] * 100.00 / OE[pos.first] << "%)" << "; ";
            miss += ME[i][pos.first];
        }
        fout << endl;

        fout << "Edge Correct/Predicted (Precision): " << CE[pos.first] * 100.00 / (CE[pos.first] + err) << "%\n";
        fout << "Edge Correct/BothV Predicted (Precision): " << CE[pos.first] * 100.00 / (CE[pos.first] + EE[3][pos.first]) << "%\n";
        fout << "Edge Correct/Total (Recall): " << CE[pos.first] * 100.00 / OE[pos.first] << "%\n";
        fout << "Edge Correct/BothV Total (Recall) " << CE[pos.first] * 100.00 / (CE[pos.first] + ME[3][pos.first]) << "%\n";
        fout << "Edge Predicted/Original: " << (CE[pos.first] + err) * 100.00 / OE[pos.first] << "%\n";
        fout << endl; //extra blank line
    }
    fout.close();
}
//Output the analysis in raw format for visualizations
void OutRaw(string lang) {
    string out_name = "../Main/Results/RemLang/Analysis/" + lang + "/" + "POS_RAW.txt";
    fout.open(out_name);
    fout << "overall" << endl;
    fout << GO.num_edges << endl << GC.num_edges << endl;
    for(int i = 0; i < 4; i++){
        fout << GE[i].num_edges << " ";
    }
    fout << endl;
    for(int i = 0; i < 4; i++){
        fout << GM[i].num_edges << " ";
    }
    fout << endl;
    for(auto pos: OE){ //Iterate over POS, value pairs in graph of Original edges
        fout << pos.first << endl;
        fout << OE[pos.first] << endl << CE[pos.first] << endl;
        for(int i = 0; i < 4; i++){
            fout << EE[i][pos.first] << " ";
        }
        fout << endl;
        for(int i = 0; i < 4; i++){
            fout << ME[i][pos.first] << " ";
        }
        fout << endl;
    }
    fout.close();
}

//count correct/extra for the output by POS category
int main()
{
    string lang = "oc-fr"; //language pair to analyze
    string file_name = "../Main/LangData/Data-" + lang + ".txt"; //get file name for original language data
    fout.open("../Main/Analysis/Tempfile.txt"); //required output file for load-data function
    GO.loadData(file_name, fout); //load data into original graph
    countpos(GO, OV, OE, fout, "Original");
    file_name = "../Main/Results/RemLang/Analysis/" + lang + "/correct.txt"; //file name for correct predictions
    GC.loadData(file_name, fout); //load data into correct graph
    countpos(GC, CV, CE, fout, "Correct Predictions");
    for(int i = 0; i < 4; i++){
        file_name = "../Main/Results/RemLang/Analysis/" + lang + "/extra" + to_string(i) + ".txt"; //file name for extra predictions
        GE[i].loadData(file_name, fout); //load data into extra graph
        countpos(GE[i], EV[i], EE[i], fout, "Extra predictions");
    }
    for(int i = 0; i < 4; i++){
        file_name = "../Main/Results/RemLang/Analysis/" + lang + "/missed" + to_string(i) + ".txt"; //file name for missed predictions
        GM[i].loadData(file_name, fout); //load data into extra graph
        countpos(GM[i], MV[i], ME[i], fout, "Missed predictions");
    }
    fout.close();

    OutReadable(lang);
    OutRaw(lang);
}