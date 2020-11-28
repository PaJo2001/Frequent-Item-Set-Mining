#include <bits/stdc++.h>
#include <chrono>

using namespace std;
using namespace std::chrono;

#define MAX INT_MAX
#define union(sets, i, j, union_set) set_union(sets[i].begin(), sets[i].end(), sets[j].begin(), sets[j].end(), union_set.begin());

string tostring(vector<int> vec, int skip)
{
    string ret;
    int n = vec.size();
    for (int i = 0; i < vec.size(); i++)
    {
        if (i == skip)
            ;
        else
            ret += to_string(vec[i]) + " ";
    }

    if (ret[ret.length() - 1] == ' ')
    {
        return ret.substr(0, ret.length() - 1);
    }
    return ret;
}

string tostring_c2(int a, int b)
{
    return (to_string(a) + " " + to_string(b));
}

vector<vector<int>> apriori_gen(int itr, vector<vector<int>> sets, unordered_map<string, int> hash)
{
    set<vector<int>> unique_items;

    for (int i = 0; i < sets.size(); i++)
    {
        sort(sets[i].begin(), sets[i].end());
        for (int j = i + 1; j < sets.size(); j++)
        {
            sort(sets[j].begin(), sets[j].end());
            vector<int> union_set((sets[i].size() + sets[j].size()) * 2);
            vector<int>::iterator it = union(sets, i, j, union_set);
            union_set.resize(it - union_set.begin());
            bool flag = true;
            int n = union_set.size();
            if (itr == n - 1)
            {
                sort(union_set.begin(), union_set.end());
                flag = true;
                for (int k = 0; k < union_set.size(); k++)
                {
                    string test = tostring(union_set, k);
                    if (hash[test] == 0)
                        flag = false;
                }
                if (flag == true)
                    unique_items.insert(union_set);
            }
        }
    }

    vector<vector<int>> ret;
    for (auto f_item : unique_items)
    {
        ret.push_back(f_item);
    }
    return ret;
}

vector<vector<int>> prune(int minsup, vector<vector<int>> C, vector<vector<int>> transaction_matrix, unordered_map<string, int> &hash)
{
    vector<string> pruned_string;
    set<vector<int>> unique_itemset;
    hash.clear();

    for (int i = 0; i < C.size(); i++)
    {
        for (int j = 0; j < transaction_matrix.size(); j++)
        {
            string item_str = "";
            int loc = 0, len = transaction_matrix[j].size(), n = C[i].size();
            for (int k = 0; k < len; k++)
            {
                if (C[i][loc] == transaction_matrix[j][k])
                    ++loc;
                if (loc == n)
                {
                    item_str = tostring(C[i], -1);
                    unique_itemset.insert(C[i]);
                    hash[item_str]++;
                    break;
                }
            }
        }
    }
    for (auto i : hash)
    {
        if (minsup > i.second)
        {
            pruned_string.push_back(i.first);
        }
    }

    for (int i = 0; i < pruned_string.size(); i++)
    {
        hash.erase(pruned_string[i]);
    }

    vector<vector<int>> ret;

    for (auto f_item : unique_itemset)
    {
        ret.push_back(f_item);
    }

    return ret;
}

int main(void)
{

    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    // Minimum support value
    int minsup = 2000;

    ifstream file("input.txt");
    vector<vector<int>> transaction_matrix;

    string seq, item;

    set<int> item_set;
    vector<int> item_vector;

    while (getline(file, seq, '\n'))
    {
        stringstream item_stream(seq);
        while (item_stream >> item)
        {
            if (item != "-1" && item != "-2")
            {
                item_set.insert(stoi(item));
            }
        }

        for (auto i : item_set)
        {
            item_vector.push_back(i);
        }
        transaction_matrix.push_back(item_vector);
        item_vector.clear();
        item_set.clear();
    }

    // Hashing for C1,C2
    unordered_map<int, int> c1;
    unordered_map<string, int> c2;
    unordered_map<string, int> frequent_itemset_map;

    auto start_time = high_resolution_clock::now();

    for (int i = 0; i < transaction_matrix.size(); i++)
    {
        for (int j = 0; j < transaction_matrix[i].size(); j++)
        {
            c1[transaction_matrix[i][j]]++;
        }
    }

    // Transaction reduction code - pruning C1
    vector<int> pruned_string;
    for (auto i : c1)
    {
        if (minsup > i.second)
        {
            pruned_string.push_back(i.first);
        }
    }

    for (int i = 0; i < pruned_string.size(); i++)
    {
        c1.erase(pruned_string[i]);
    }
    for (int i = 0; i < transaction_matrix.size(); i++)
    {
        int len = transaction_matrix[i].size();
        for (int j = 0; j < len; j++)
        {
            for (int k = j + 1; k < len; k++)
            {
                if (c1[transaction_matrix[i][k]] != 0 && c1[transaction_matrix[i][j]] != 0)
                {
                    c2[tostring_c2(transaction_matrix[i][j], transaction_matrix[i][k])]++;
                }
            }
        }
    }

    // Transaction reduction code - pruning C2
    vector<string> pruned_string2;
    for (auto i : c2)
    {
        if (i.second < minsup)
        {
            pruned_string2.push_back(i.first);
        }
    }

    for (int i = 0; i < pruned_string2.size(); i++)
    {
        c2.erase(pruned_string2[i]);
    }

    // Running apriori beyond C2
    vector<vector<int>> C, sets;
    vector<int> sequence;
    string item1;
    for (auto i : c2)
    {
        sequence.clear();
        item1 = "";
        stringstream item_stream(i.first);
        while (item_stream >> item1)
        {
            sequence.push_back(stoi(item1));
        }
        sets.push_back(sequence);
    }

    int itr = 2;

    C = apriori_gen(itr++, sets, c2); //for itr==2

    while (!C.empty())
    {
        frequent_itemset_map.clear();
        sets = prune(minsup, C, transaction_matrix, frequent_itemset_map);
        C.clear();
        C = apriori_gen(itr++, sets, frequent_itemset_map);
    }

    auto end_time = high_resolution_clock::now();
    auto time_taken_sec = (duration_cast<seconds>(end_time - start_time)).count();
    auto time_taken_msec = (duration_cast<milliseconds>(end_time - start_time)).count();

    ofstream fout;
    fout.open("frequent_apriori_output.txt");
    cout << "Final Iteration Frequent ItemSets are Given Below -- Format (ItemSet : Frequency) -- (MinSup:" << minsup << ")" << endl;
    fout << "Final Iteration Frequent ItemSets are Given Below -- Format (ItemSet : Frequency) -- (MinSup:" << minsup << ")" << endl;
    for (auto ix : frequent_itemset_map)
    {
        cout << ix.first << "  :  " << ix.second << endl;
        fout << ix.first << "  :  " << ix.second << endl;
    }
    cout << "Time taken (sec) :" << time_taken_sec << endl;
    cout << "Time taken (ms):" << time_taken_msec << endl;
    fout << "Time taken (sec) :" << time_taken_sec << endl;
    fout << "Time taken (ms) :" << time_taken_msec << endl;
    fout.close();

    return 0;
}
