#include<iostream>
#include<string>
#include<fstream>
#include<unordered_map>
#include<vector>
#include<chrono>
#include<bits/stdc++.h>
using namespace std;

bool newIsFeasibleOrdering(vector<string> *currentPoints, int);
bool candidateOrderingIsExtendable(vector<string>,int);

vector<string> viewpointsAH;

int main(int argc, char *argv[]){
    ifstream viewPointInput;
    viewPointInput.open("vplist.txt");

    // read vplist.txt into viewpointsAH vector
    while(!viewPointInput.eof()){
        string thisVP = "";
        while(viewPointInput.peek()!='\n')
        {
            thisVP += viewPointInput.get();
        }
        viewPointInput.get();
        viewpointsAH.push_back(thisVP);
        viewPointInput.peek();
    }
    viewPointInput.close();
    
    for(int i = (atoi(argv[1])); i <= 11880; i = i + 23) {
        // process case by case
        string fileName = "../Unsolved/case" + to_string(i) + ".txt";
        ifstream input;
        input.open(fileName);
        
        vector<string> initialInput;
        if(input.fail()){
            // skip
            // cout << "Could not open " << fileName << endl;
            continue;
        }
        
        // read case_i.txt to initialInput vector
        while(!input.eof()){
            string thisPoint = "";
            while(input.peek()!='\n')
                thisPoint += input.get();
            input.get();
            initialInput.push_back(thisPoint);
            input.peek();
        }
        input.close();
        
        time_t start,end;
        time(&start);

        if(!candidateOrderingIsExtendable(initialInput,i)){
            time(&end);
            double runningTime = double(end - start);
            cout << "Case " << i << " couldn't fit all " << viewpointsAH.size() << " points.  Running Time: " << setprecision(2) << runningTime << " seconds." << endl;
            string mvString = "mv " + fileName + " ../Orange256/";
            if(system(mvString.c_str())){
                cout << "Couldn't move file" << endl;
                return -1;
            }
        }
        else{
            time(&end);
            double runningTime = double(end - start);
            cout << "Case " << i << " placed all " << viewpointsAH.size() << " points.  RT: " << setprecision(2) << runningTime << " seconds." << endl;
        }
    }
    
	return 0;
}


//Return true if everything is placed.  Return false if not everything is placed.
bool candidateOrderingIsExtendable(vector<string> inputPoints,int caseNumber){ 
    unordered_map<string,bool> usedVP;
    
    for(int i=0; i<inputPoints.size(); i++){
        if(inputPoints[i].size() > 1)
        {
            // remove the viewpoints in inputPoints vector (mark them as used), we always try to recursively insert unused viewpoints from viewpointsAH
            usedVP[inputPoints[i]] = true;
        }
    }
    
    vector<string> viewPointList;
    for(int i=0; i<viewpointsAH.size(); i++){
        if(!usedVP[viewpointsAH[i]])
            viewPointList.push_back(viewpointsAH[i]);
    }
        
    if(viewPointList.size() == 0){
        // no more unused viewpoint to insert
        cout << "We placed everything!" << endl;
        newIsFeasibleOrdering(&inputPoints,caseNumber);        
        return true;
    }
    
    // always pick the viewpoint that has minimum gap could make current ordering feasible
    int minGaps = inputPoints.size() + 2;
    string optVP;
    vector<int> optimalGaps;
    
    for(int z=0; z < viewPointList.size(); z++){
        // check every unused viewpoints in viewPointList
        int correctGaps = 0;
        vector<int> feasibleGaps;
        
        for(int gap = 0; (gap <= inputPoints.size()) && (correctGaps < minGaps); gap++){
            vector<string> currentPoints;
            int index = 0;
            
            // insert this viewpoint into currentPoints in every position
            while(index < gap){
                currentPoints.push_back(inputPoints[index]);
                index++;
            }
            currentPoints.push_back(viewPointList[z]);
            while(index<inputPoints.size()){
                currentPoints.push_back(inputPoints[index]);
                index++;
            }
            
            // intermediate checking, store the number of gaps and which gaps are feasible
            if(newIsFeasibleOrdering(&currentPoints, 0)){
                correctGaps++;
                feasibleGaps.push_back(gap);
            }
        }
        
        // find the optimal one that has the minimum number of gaps could make current ordering feasible
        if(correctGaps<minGaps){
            minGaps = correctGaps;
            optVP = viewPointList[z];
            optimalGaps = feasibleGaps;
        }
        
    }
      
    if(minGaps == 0){
        // Found a point that cannot go into the previous config.
        // cout << "Couldn't place " << optVP << endl;
        return false;
    }
    
    else{
        // recursively call candidateOrderingIsExtendable with this new added optimal viewpoint in every gaps, not only the feasible gaps
        // cout << "optimal gaps size: " << optimalGaps.size() << endl;
        // cout << "input points size: " << inputPoints.size() << endl;
        
        for(int i=0; i<optimalGaps.size(); i++){
            
            int gap = optimalGaps[i];
            vector<string> nextInput;
     
            int index = 0;
            while(index < gap){
                nextInput.push_back(inputPoints[index]);
                index++;
            }
            nextInput.push_back(optVP);
            while(index<inputPoints.size()){
                nextInput.push_back(inputPoints[index]);
                index++;
            }
            
            if(candidateOrderingIsExtendable(nextInput,caseNumber))
            {
                return true;
            }
        }
    }
    
    return false;
}

bool newIsFeasibleOrdering(vector<string> *currentPoints, int caseNumber){
    // cout << "in newIsFeasibleOrdering" << endl;
    ofstream output;
    if(caseNumber != 0){
     
        string fn = "output" + to_string(caseNumber) + ".txt";
        output.open(fn);
        output << "Case " << caseNumber << endl;
        for(int i=0; i<(*currentPoints).size(); i++)
            output << (*currentPoints)[i] << endl;
        
    }
    
    vector<int> guardIndices;
    int n = (*currentPoints).size();
    
    for(int i=0; i<(*currentPoints).size(); i++){
        
        if((*currentPoints)[i].size() == 1)
            guardIndices.push_back(i);
    }
    
    
    unordered_map<string,bool> cannotBlockWithTerrain;
    for(int i=0; i<(*currentPoints).size(); i++){

        int isAGuard = 1;
        if((*currentPoints)[i].size() != 1)
            isAGuard = 0;

        for(int j=i+1; j<(*currentPoints).size(); j++){
            
            string hashKey = (*currentPoints)[i] + "," + (*currentPoints)[j];
            bool cannotBlock = false;

            int jGuard = 1;
            if((*currentPoints)[j].size() != 1)
                jGuard = 0;

            if(isAGuard + jGuard == 1){

                
                if(isAGuard){
                    int check = (*currentPoints)[j].find((*currentPoints)[i]);

                    if(check >= 0){

                        //cout << currentPoints[i] << " sees " << currentPoints[j] << endl;
                        cannotBlock = true;
                    }
                }
                else{
                
                    int check = (*currentPoints)[i].find((*currentPoints)[j]);
                    
                    if(check >= 0){
                        //cout << (*currentPoints)[i] << " sees " << (*currentPoints)[j] << endl;
                        cannotBlock = true;
                    }                    
                    
                }
            }
            
            cannotBlockWithTerrain[hashKey] = cannotBlock;


        }

    }
    
    
    
    //Do order claims.
    for(int dist=3; dist <(*currentPoints).size(); dist++){
        
        for(int a=0; a<(*currentPoints).size(); a++){
            
            bool foundAnOC = false;
            
            int d = a + dist;
            
            if(d>=(*currentPoints).size())
                continue;
            
            for(int b = a+1; b<d-1; b++){
            
                for(int c = b+1; !foundAnOC && c<d; c++){
                    
                
                    string acHashKey = (*currentPoints)[a] + "," + (*currentPoints)[c];
                    string bdHashKey = (*currentPoints)[b] + "," + (*currentPoints)[d];
                    string adHashKey = (*currentPoints)[a] + "," + (*currentPoints)[d];
                    
                    if(cannotBlockWithTerrain[acHashKey] && cannotBlockWithTerrain[bdHashKey]){
                    
                        //cout << "cannot block " <<*currentPoints[a] << " from " << (*currentPoints)[d] << " with terrain by OC." << endl;
                        
                        cannotBlockWithTerrain[adHashKey] = true;
                        foundAnOC = true;
                        
                    }
                    
                    
                }
                
                
                
            }
        }
        
        
    }
    //End Order Claims.
    
    //cout << "Have order claims" << endl;
    
    //For each guard, check left and right, but remember the key points.
    vector<int> rightMostPointGuardSees, leftMostPointGuardSees, closestPointDontSeeWithOCLeft, closestPointDontSeeWithOCRight;
    
    
    for(int i=0; i<guardIndices.size(); i++){
        
        
        int gI = guardIndices[i];
        string gS = (*currentPoints)[gI];
        //cout << "Checking guard " << gS << endl;
        
        //Check to Right.
        bool foundPointDontSeeWithOC = false;
        int rightMostPointWeSee = gI, firstRightOC = (*currentPoints).size();
        for(int vp = gI+1; vp<(*currentPoints).size(); vp++){
         
            if((*currentPoints)[vp].size() == 1)
                continue;
            
            int check = (*currentPoints)[vp].find(gS);
            if(check >= 0){
                
                if(foundPointDontSeeWithOC){
                    //cout << "Returning false because " << gS << " sees " << (*currentPoints)[vp] << " to the right of " << (*currentPoints)[firstRightOC] << endl;
                    return false;
                }
                
                rightMostPointWeSee = vp;
            }
            else if(!foundPointDontSeeWithOC && cannotBlockWithTerrain[gS +"," + (*currentPoints)[vp]]){
                
                int leftMostGuardPointSees = (*currentPoints)[vp][0] - 'A';
                
                if(leftMostGuardPointSees < i){
                    foundPointDontSeeWithOC = true;
                    firstRightOC = vp;
                }
            }
            
        }
        
        //if(gS == "A") cout << "rightMostSees: " << rightMostPointWeSee << ", closest unpiercable point we dont see to right: " << firstRightOC;
        
        rightMostPointGuardSees.push_back(rightMostPointWeSee);
        closestPointDontSeeWithOCRight.push_back(firstRightOC);
        
        
        //Check to Left.
        foundPointDontSeeWithOC = false;
        int leftMostPointWeSee = gI, firstLeftOC = -1;
        for(int vp = gI-1; vp>=0; vp--){
            
            if((*currentPoints)[vp].size() == 1)
                continue;
         
            int check = (*currentPoints)[vp].find(gS);
            if(check >= 0){
                
                if(foundPointDontSeeWithOC){
                    //cout << "Returning false because " << gS << " sees " << (*currentPoints)[vp] << " to the left of " << (*currentPoints)[firstLeftOC] << endl;
                    return false;
                }
                
                leftMostPointWeSee = vp;
            }
            else if(!foundPointDontSeeWithOC && cannotBlockWithTerrain[(*currentPoints)[vp] +"," + gS]){
                
                //if(gS == "A") cout << "@@@@@@@@@@@ " << (*currentPoints)[vp] << endl;
                int rightMostGuardPointSees = (*currentPoints)[vp][(*currentPoints)[vp].size()-1] - 'A';
                
                if(rightMostGuardPointSees > i){
                    foundPointDontSeeWithOC = true;
                    firstLeftOC = vp;
                }
            }
            
        }
        
        //if(gS == "A") cout << ", leftMostSees: " << leftMostPointWeSee << ", closest unpiercable point we dont see to left: " << firstLeftOC << endl;
        
        leftMostPointGuardSees.push_back(leftMostPointWeSee);
        closestPointDontSeeWithOCLeft.push_back(firstLeftOC);
    }
    
   // cout << "checked right and left" << endl;
   if(caseNumber == 1038){
       
       for(int z=0; z<guardIndices.size(); z++){
           output << "Guard " << (*currentPoints)[guardIndices[z]] << endl;
           output << "leftMost: " << leftMostPointGuardSees[z] << endl;
           output << "rightMost: " << rightMostPointGuardSees[z] << endl;
       }
   }
    
    //Higher guard.
    unordered_map<string,string> higherGuard;
    unordered_map<string,string> insideEachOthersDisks;
    
    for(int i=0; i<guardIndices.size(); i++){
        
        int g1I = guardIndices[i];
        string g1S = (*currentPoints)[g1I];
        
        for(int j= i+1; j<guardIndices.size(); j++){
            
            int g2I = guardIndices[j];
            string g2S = (*currentPoints)[g2I];
            
            //bool insideDisks = true;
            
            string highGuardKey = g1S + "," + g2S;
            higherGuard[highGuardKey] = "?";
            insideEachOthersDisks[highGuardKey] = "?";
            if(leftMostPointGuardSees[j] <= g1I && rightMostPointGuardSees[i] >= g2I){
                insideEachOthersDisks[highGuardKey] = "yes";
            }
            
            if(leftMostPointGuardSees[j] <= closestPointDontSeeWithOCLeft[i]){
                /*if(g1S == "A"){
                    cout << "$$$$$$$$$$$$$$$ " << leftMostPointGuardSees[j] << " <= " << closestPointDontSeeWithOCLeft[i] << endl;
                    for(int z=0; z<closestPointDontSeeWithOCLeft.size(); z++){
                        cout << closestPointDontSeeWithOCLeft[z] << ", ";
                    }
                    cout << endl;
                }*/
                higherGuard[highGuardKey] = g2S;
            }
            
            if(rightMostPointGuardSees[i] >= closestPointDontSeeWithOCRight[j]){
                
                if(higherGuard[highGuardKey] == g2S){
                    //cout << "Returning false because " << g1S << " and " << g2S << " are higher than each other." << endl;
                    return false;
                }
                
                higherGuard[highGuardKey] = g1S;
            }
            
            
            if(closestPointDontSeeWithOCRight[i] < g2I && cannotBlockWithTerrain[highGuardKey]){
                
                //if(g1S == "A")
                    //cout << "********" << closestPointDontSeeWithOCRight[i] << " < " << g2I << endl;
             
                if(higherGuard[highGuardKey] == g1S){
                    //cout << "Returning false because " << g1S << " and " << g2S << " are higher than each other." << endl;
                    return false;
                }
                
                //insideDisks = false;
                insideEachOthersDisks[highGuardKey] = "no";
                higherGuard[highGuardKey] = g2S;
            }
            
            if(closestPointDontSeeWithOCLeft[j] > g1I && cannotBlockWithTerrain[highGuardKey]){
             
                if(higherGuard[highGuardKey] == g2S){
                    //cout << "Returning false because " << g1S << " and " << g2S << " are higher than each other." << endl;
                    return false;
                }
                
                //insideDisks = false;
                insideEachOthersDisks[highGuardKey] = "no";
                higherGuard[highGuardKey] = g1S;
                
            }
            
            //insideEachOthersDisks[highGuardKey] = insideDisks;
            
        }
        
    }
    //End Higher Guard
    
    //cout << "checked higher guard" << endl;
    
    //If g1 and g2 shouldn't be blocked by terrain, then no midguard can be higher than both of them.
    for(int i=0; i<guardIndices.size(); i++){
        
        int g1I = guardIndices[i];
        string g1S = (*currentPoints)[g1I];
        
        for(int j= i+2; j<guardIndices.size(); j++){
            
            int g2I = guardIndices[j];
            string g2S = (*currentPoints)[g2I];
            
            if(!cannotBlockWithTerrain[g1S + "," + g2S])
                continue;
            
            for(int k=i+1; k<j; k++){
             
                int midGuardI = guardIndices[k];
                string midGuardS = (*currentPoints)[midGuardI];
                
                if(higherGuard[g1S + "," + midGuardS] == midGuardS && higherGuard[midGuardS + "," + g2S] == midGuardS){
                    //cout << "Returning false because " << midGuardS << " is higher than " << g1S << " and " << g2S << "." << endl;
                    return false;
                }
                
            }
            
        }
        
        
    }
    //End midGuard higher than both.
    
    //cout << "midguard higher done" << endl;
    
    //Check to see if a guard is outside of a disk but needs to block to points that are inside the disk.
    for(int i=0; i<guardIndices.size(); i++){
        
        int g1I = guardIndices[i];
        string g1S = (*currentPoints)[g1I];
        
        if(caseNumber == 1038){
            
            output << "g1: " << g1S << endl;
        }
        
        for(int j= i+1; j<guardIndices.size(); j++){
            
            int g2I = guardIndices[j];
            string g2S = (*currentPoints)[g2I];
            
            if(caseNumber == 1038){
            
                output << "g2: " << g2S << endl;
            }
            
            string hashKey = g1S + "," + g2S;
            
            if(caseNumber == 1038 && g1S == "E" && g2S == "F"){
                
                output << "hashKey: " << hashKey << endl;
                output << "cannotBlockWithTerrain: " << cannotBlockWithTerrain[hashKey] << endl;
            }
            
            
            if(!cannotBlockWithTerrain[hashKey])// || insideEachOthersDisks[hashKey] == "yes")
                continue;
            
            //cout << "Checking higher of " << g1S << " and " << g2S << endl;
            //if(higherGuard[hashKey] == g2S){
                
                //cout << g2S << " is higher" << endl;                
             
                //Outside of each other's disks, and g2 is higher.
            for(int vp = g1I-1; vp>=0; vp--){
                
                string vpS = (*currentPoints)[vp];
                if(vpS.size() == 1)
                    continue;
                
                //vp can be too far away from g2
                if(leftMostPointGuardSees[j] > vp)
                    break;
                if(j >= int(vpS.at(vpS.length() - 1) - 'A'))
                {
                    continue; // check both side
                }
                    
                int check = vpS.find(g2S);
                if(check >= 0)
                    break;
                else{
                    
                    bool blockerCanBeG1, blockerCanBeNewPoint;
                    
                    
                    //Is there a disc that contains g2 and vp but does not contain g1?
                
                    blockerCanBeG1 = true;
                    for(int k=0; k<guardIndices.size(); k++){
                        
                        if(k == i)
                            continue;
                        
                        int gkI = guardIndices[k];
                        string gkS = (*currentPoints)[gkI];
                        string firstHash;
                        if(k<i)
                            firstHash = gkS + "," + g1S;
                        else
                            firstHash = g1S + "," + gkS;
                        
                        if(insideEachOthersDisks[firstHash] != "no")
                            continue;
                        
                        string secondHash;
                        if(k<j)
                            secondHash = gkS + "," + g2S;
                        else
                            secondHash = g2S + "," + gkS;
                        
                        if(k != j && insideEachOthersDisks[secondHash]!="yes")
                            continue;
                        
                        //bool vpMustBeInDiscK = false; k < vp < g1 < g2
                        if(gkI < vp && rightMostPointGuardSees[k] >= vp){
                            
                            int leftMostGuardPointSees = vpS[0] - 'A';
                            
                            if(leftMostGuardPointSees <= k){
                                blockerCanBeG1 =  false;
                                break;
                            }
                        }
                        else if((gkI > vp && leftMostPointGuardSees[k] <= vp) || (gkI > g2I && leftMostPointGuardSees[k] > vp && cannotBlockWithTerrain[vpS +","+gkS] && higherGuard[g1S + "," + gkS] == gkS)){
                            
                            int rightMostGuardPointSees = vpS[vpS.size()-1] - 'A';
                            if(rightMostGuardPointSees >= k){
                                blockerCanBeG1 =  false;
                                break;
                            }
                        }
                        
                        
                    }
                    
                    if(blockerCanBeG1)
                        continue;
                    
                    
                    if(cannotBlockWithTerrain[vpS + "," + g1S]){
                        blockerCanBeNewPoint = false;
                    }
                    else{
                        //New blocker may make more things unpiercable
                        // new oc: (vp2 ,point between vp and g1,g1 , g2) -> vp2, g2 \oc -> vp2 low outside of g2's disk -> vp2 sees g' right to g2 -> unrealizable
                        blockerCanBeNewPoint = true;
                        for(int vp2 = vp-1; vp2 > leftMostPointGuardSees[j]; vp2--){
                            
                            check = (*currentPoints)[vp2].find(g1S);
                            if(check >=0){
                             
                                check = (*currentPoints)[vp2].find(g2S);
                                if(check < 0){
                                    
                                    int rightMostGuardPointSees = (*currentPoints)[vp2][(*currentPoints)[vp2].size()-1] - 'A';
                                    
                                    if(rightMostGuardPointSees > j){
                                        blockerCanBeNewPoint = false;
                                        break;
                                    }
                                }
                            }
                            
                            
                        }
                    }
                    
                    
                    if(!blockerCanBeNewPoint)
                        return false;
                   
                    
                    
                }
                
            }
            
            for(int vp = g2I+1; vp<(*currentPoints).size(); vp++){
                
                
                if(caseNumber == 1038 && g1S == "E" && g2S == "F" && (*currentPoints)[vp] == "ABCFGH"){
                
                    output << "vp: " << vp << endl;
                    output << "rightMostPointGuardSees[i]: " << rightMostPointGuardSees[i] << endl;
                    output << "hashKey: " << g2S + "," + (*currentPoints)[vp] << endl;
                    output << "cannotBlockWithTerrain[g2S + \",\" + (*currentPoints)[vp]]" << cannotBlockWithTerrain[g2S + "," + (*currentPoints)[vp]] << endl;
                }
                
                string vpS = (*currentPoints)[vp];
                if(vpS.size() == 1)
                    continue;
                
                //vp can be too far away from g2
                if(rightMostPointGuardSees[i] < vp)
                    break;

                if(i <= int(vpS.at(0) - 'A'))
                {
                    continue; 
                }
                    
                int check = vpS.find(g1S);
                if(check >= 0)
                    break;
                else{
                    
                    bool blockerCanBeG2, blockerCanBeNewPoint;
                    
                    //Is there a disc that contains g1 and vp but does not contain g2?
                
                    blockerCanBeG2 = true;
                
                    for(int k=0; k<guardIndices.size(); k++){
                        
                        if(k == j)
                            continue;
                        
                        int gkI = guardIndices[k];
                        string gkS = (*currentPoints)[gkI];
                        string firstHash;
                        if(k<i)
                            firstHash = gkS + "," + g1S;
                        else
                            firstHash = g1S + "," + gkS;
                        
                        
                        
                        string secondHash;
                        if(k<j)
                            secondHash = gkS + "," + g2S;
                        else
                            secondHash = g2S + "," + gkS;
                        
                        
                        if(caseNumber == 1038 && g1S == "E" && g2S == "F" && (*currentPoints)[vp] == "ABCFGH" && gkS == "A"){
                
                            output << "firstHash: " << firstHash << endl;
                            output << "insideEachOthersDisks[firstHash]: " << insideEachOthersDisks[firstHash] << endl;
                            output << "secondHash: " << secondHash << endl;
                            output << "insideEachOthersDisks[secondHash]: " << insideEachOthersDisks[secondHash] << endl;
                            output << "rightMostPointGuardSees[k]: " << rightMostPointGuardSees[k] << endl;
                        }
                        
                        if(k!=i && insideEachOthersDisks[firstHash] != "yes")
                            continue;
                        
                        if(insideEachOthersDisks[secondHash] != "no")
                            continue;
                        
                        //bool vpMustBeInDiscK = false;
                        if((gkI < vp && rightMostPointGuardSees[k] >= vp) || (gkI < g1I && rightMostPointGuardSees[k] < vp && cannotBlockWithTerrain[gkS +"," + vpS] && higherGuard[gkS + "," + g2S] == gkS) ){
                            
                            int leftMostGuardPointSees = vpS[0] - 'A';
                            
                            if(leftMostGuardPointSees <= k){
                                blockerCanBeG2 = false;
                                break;
                            }
                        }
                        else if(gkI > vp && leftMostPointGuardSees[k] <= vp){
                            
                            int rightMostGuardPointSees = vpS[vpS.size()-1]-'A';
                            if(rightMostGuardPointSees >= k){
                                blockerCanBeG2 = false;
                                break;
                            }
                        }
                        
                    }
                    
                    if(blockerCanBeG2)
                        continue;
                    
                    
                    if(cannotBlockWithTerrain[g2S + "," + (*currentPoints)[vp]]){
                        blockerCanBeNewPoint = false;
                    }
                    else{
                        //New blocker may make more things unpiercable
                        blockerCanBeNewPoint = true;
                        for(int vp2 = vp+1; vp2 < rightMostPointGuardSees[i]; vp2++){
                            
                            check = (*currentPoints)[vp2].find(g2S);
                            if(check >=0){
                             
                                check = (*currentPoints)[vp2].find(g1S);
                                if(check < 0){
                                    
                                    int leftMostGuardPointSees = (*currentPoints)[vp2][0] - 'A';
                                    if(leftMostGuardPointSees < i){
                                        blockerCanBeNewPoint = false;
                                        break;
                                    }
                                }
                            }
                            
                            
                        }
                    }
                    
                    
                    if(!blockerCanBeNewPoint)
                        return false;
                    
                    
                }
                
            }
            
        }
        
    }
    
    for(int i=0; i<guardIndices.size(); i++){
        int g1I = guardIndices[i];
        string g1S = (*currentPoints)[g1I];
        
        for(int j= i+1; j<guardIndices.size(); j++){
            
            int g2I = guardIndices[j];
            string g2S = (*currentPoints)[g2I];
            
            //string hashKey = g1S + "," + g2S;
            
            for(int vp2 = g1I + 1; vp2 < g2I; vp2++){
               string vp2S = (*currentPoints)[vp2];
               int check = vp2S.find(g1S);
               if(check >= 0) // vp2 does not see g1
                    continue;
               check = vp2S.find(g2S);
               if(check < 0) // vp2 sees g2
                    continue;
                          
               string hashKey2 = g1S + "," + vp2S;
               if(!cannotBlockWithTerrain[hashKey2]) // oc on g1 and vp2
                    continue;
               
               if(rightMostPointGuardSees[i] <= vp2) // g1 sees right to the vp2
                    continue;
               
               for(int vp1 = vp2 +1; vp1 < g2I; vp1++) {
                    string vp1S = (*currentPoints)[vp1];
                    check = vp1S.find(g1S);
                    if(check < 0) // vp1 sees g1
                        continue;
                    check = vp1S.find(g2S);
                    if(check >= 0) // vp1 does not see g2
                        continue;
                    
                    string hashKey1 = vp1S + "," + g2S;
                    
                    if(!cannotBlockWithTerrain[hashKey1]) // oc on vp1 and g2
                        continue;
                    
                    if(leftMostPointGuardSees[j] >= vp1) // g2 sees left to the vp1
                        continue;
                    
                    return false;
               }
            }
        }
    }

    output.close();
    return true;
    
}
