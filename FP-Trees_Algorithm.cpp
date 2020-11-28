#include<bits/stdc++.h>
#include <chrono> 

using namespace std;
using namespace std::chrono;



#define ll long long
#define ff first
#define ss second
#define vll vector<ll>
#define vpll vector<pair<ll,ll>>
#define rep(i,m,n) for(ll i=m;i<n;i++)
#define pb push_back	

const ll NUM_ITEM_NAMES = 2500;


vector<pair<vll,ll>> ans;
map <ll,ll> fqMap;
vpll fqVec;
queue<ll> queue_txn;



class fpTree{

    public:
        ll count=0;
        ll itemName;
        fpTree* parent;
        fpTree* link[NUM_ITEM_NAMES];

    fpTree(ll ch)
    {
        itemName=ch;
        count = 1;
        parent = nullptr;
        rep(i,0,NUM_ITEM_NAMES)
        {
            link[i]=nullptr;
        }
    }
};

class Transaction{

    public:
        vector <ll> items;

    Transaction( vll itt)
    {
        items=itt;
    }
};

class Header_table{
    public: 
        ll itemName;
        fpTree* target;
        Header_table* next;

    Header_table(ll cc)
    {
        itemName=cc;
        target=nullptr;
        next=nullptr;
    }

};

ll chknode=0;
ll buildch=0;
map<ll,pair<Header_table*,ll>>hdr,curr,cc2;

vll formset(string a, ll item)
{
    vll temp;
    string word;
    stringstream ss(a);
    
    while(ss>>word)
    {
        ll num=stoi(word);
        temp.pb(num);
    }
    temp.pb(item);

    return temp;
}


void insert_tree(queue<ll> &queue_txn, fpTree* node)
{    
    ll cnt=0;
    if(queue_txn.empty())
    {
        return;
    }
    bool flag=0;
    rep(u,0,NUM_ITEM_NAMES)
    {
        if(node->link[u]&&node->link[u]->itemName == queue_txn.front())
        {
            queue_txn.pop();
            node->link[u]->count++;
            insert_tree(queue_txn,node->link[u]);
            chknode++;
            cnt++;
            flag=1;
            hdr[node->link[u]->itemName].ss++;
            break;
        }
    }
   
    if(!flag && cnt==0)
    {
        fpTree* nn = new fpTree(queue_txn.front());   
        queue_txn.pop();
        nn->parent = node;
        ll valr=nn->itemName;
        node->link[(nn->itemName)]=nn;
        Header_table* hh = new Header_table(nn->itemName);
        hh->target = nn;
        if(hdr.find(nn->itemName)!=hdr.end())
        {
            curr[nn->itemName].ff->next =  hh;
            valr=hh->itemName;
            curr[nn->itemName] = {hh,nn->count};
            hdr[nn->itemName].ss++;    
        }
        else 
        {
            hdr[nn->itemName]={hh,nn->count};
            valr=hh->itemName;
            curr[nn->itemName]={hh,nn->count};
        }
   
        insert_tree(queue_txn,node->link[(nn->itemName)]);
    }
    else
        return;
    return;
}


void build_tab(map<ll,pair<Header_table*,ll>> &ret,Header_table* curr,ll fg)
{
    cc2.clear();
    ret.clear();
    map<ll,ll>badmap;
    buildch++;
    while(curr!=nullptr)
    {
        fpTree* walk = curr->target->parent;
        while(walk->itemName!=0)
        {
            if(badmap.find(walk->itemName)==badmap.end())
            {
                walk->count=0;
                badmap[walk->itemName]=1;
            }
            Header_table* hh = new Header_table(walk->itemName);
            hh->target = walk;
            if(ret.find(walk->itemName)!=ret.end())
            {
                ll valr=hh->itemName;
                cc2[walk->itemName].ff->next =  hh;
                if(fg)
                {
                    fg--;
                }
                cc2[walk->itemName] = {hh,walk->count+fg};
                ll valcc=cc2[walk->itemName].ss;
                ret[walk->itemName].ss+=curr->target->count;
            }
            else 
            {
                ret[walk->itemName]={hh,curr->target->count};
                cc2[walk->itemName]={hh,curr->target->count};
            }
            if(fg)
            {
                fg--;
            }
            walk=walk->parent;
            if(walk->itemName==0)
                break;
        }
        curr=curr->next;
        if(curr==nullptr)
            break;
    }
}

void mineTree(string pattern, ll minsup, map<ll,pair<Header_table*,ll>> H)
{
    map<ll,pair<Header_table*,ll>>tmap;
    ll fg=1;
    for(auto ix:H)
    {
        if(ix.ss.ss<minsup)
            continue;
        else
        {
            auto temp1=formset(pattern,ix.ff);
            ans.pb({temp1,ix.ss.ss});
            tmap.clear();
            fg=0;
            build_tab(tmap,H[ix.ff].ff,fg);
            mineTree(pattern + ' ' + to_string(ix.ff), minsup, tmap);
        }
    }
}

int main()
{    
    vll v1;
    ll num;
    set<ll>s1;
    ans.clear();
    vector<Transaction> txn;
    ll minsup = 2000;
    string str;
    string word;
    vector<vll> arr;

    ifstream file("input.txt");
    while(getline(file, str, '\n'))
    {
        stringstream ss(str);
        s1.clear();
        while(ss >> word)
        {
            if(word == "-1" || word == "-2")
            {
                continue;
            }
            num=stoi(word);
            s1.insert(num);
        }
        v1.clear();
        for(auto u:s1)
        {
            v1.pb(u);
        }
        arr.pb(v1);
    }
    ll sz=arr.size();
    rep(i,0,sz)
    {
        Transaction T(arr[i]);
        txn.pb(T);
    }
   
    ll sz_txn=txn.size();
    rep(i,0,sz_txn)
    {
        ll sz_txn_items = txn[i].items.size(); 
        rep(j,0,sz_txn_items)
        {
            fqMap[txn[i].items[j]]++;
        }
    }
    for(auto u:fqMap)
    {
        if(u.ss<minsup)
            continue;
        else
            fqVec.pb({u.ff,u.ss});
    }

    auto start = high_resolution_clock::now();

    set<ll> tempSet;
    fpTree* root = new fpTree(0);
    root->parent = nullptr;


    rep(i,0,sz_txn)
    {

        while(!queue_txn.empty())
        {
            queue_txn.pop();
        }
        tempSet.clear();

        ll sz_txn_items = txn[i].items.size();
        ll sz_fqVec = fqVec.size();
        rep(j,0,sz_txn_items)
        {
            tempSet.insert(txn[i].items[j]);
        }
        rep(j,0,sz_fqVec)
        {
            if(tempSet.find(fqVec[j].ff)==tempSet.end())
                continue;
            else
                queue_txn.push(fqVec[j].ff);
        }
        insert_tree(queue_txn,root);
    }
    
    mineTree("",minsup,hdr);

    auto stop = high_resolution_clock::now();

    ofstream fout;
    fout.open("frequent_fpgrowth.txt");
    fout<<"Finally, frequent itemsets and their frequencies are:\n";
    ll sz_ans=ans.size();
    rep(i,0,sz_ans)
    {
        ll sz_ans_i = ans[i].ff.size();
        rep(j,0,sz_ans_i)
        {
            fout<<ans[i].ff[j]<<' ';
        }
        ll val=ans[i].ss;
        fout<<':'<<val<<'\n';
    }

    auto duration = duration_cast<seconds>(stop - start);
    cout <<"Time taken in seconds:"<< duration.count() << endl;
    // fout <<"Time taken in seconds:"<< duration.count() << endl;
    auto durationmin = duration_cast<milliseconds>(stop - start); 
    cout <<"Time taken in millis:"<< durationmin.count() << endl;  
    // fout <<"Time taken in millis:"<< durationmin.count() << endl;  
    fout.close();

    return 0;
}
