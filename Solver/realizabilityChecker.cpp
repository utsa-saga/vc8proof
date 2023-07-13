#include<iostream>
#include<string>
#include<fstream>
#include<unordered_map>
#include<vector>
#include<chrono>
#include<bits/stdc++.h>
using namespace std;

bool candidateOrderingIsExtendable(vector<string>,int);
bool isNotRealizable(vector<string> *currentPoints, int);
string generateHashKey(int index1, int index2, vector<string> *arr);

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
            string mvString = "mv " + fileName + " ../Solved/";
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
        isNotRealizable(&inputPoints,caseNumber);
        // isNotRealizable(&inputPoints)   
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
            if(!isNotRealizable(&currentPoints, 0)){
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

bool isNotRealizable(vector<string> *currentPoints, int caseNumber){
    ofstream output;
    if(caseNumber != 0){
        string fn = "output" + to_string(caseNumber) + ".txt";
        output.open(fn);
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
            if(d>=(*currentPoints).size()){
                continue;
            }
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
    
    
    //For each guard, check left and right, but remember the key points. (index)
    vector<int> rightMostPointGuardSees, leftMostPointGuardSees, closestPointHighOutsideLeft, closestPointHighOutsideRight;
    
    for(int i=0; i<guardIndices.size(); i++){
        int gI = guardIndices[i]; // the index of the guard in currentPoints
        string gS = (*currentPoints)[gI]; // the name of guard
        //Check to Right.
        bool foundPointDontSeeWithOC = false;
        int rightMostPointWeSee = gI, firstRightViewpointHighOutside = (*currentPoints).size();
        for(int vp = gI+1; vp<(*currentPoints).size(); vp++){
            if((*currentPoints)[vp].size() == 1) // find guard
                continue;
            int check = (*currentPoints)[vp].find(gS);

            if(check >= 0){
                if(foundPointDontSeeWithOC){
                    // x(closestPointHighOutsideRight) < x(rightMostGuardPointSees)
                    return true;
                }
                rightMostPointWeSee = vp;
            }

            else if(!foundPointDontSeeWithOC && cannotBlockWithTerrain[gS +"," + (*currentPoints)[vp]]){ // vp does not see g and pierced
                int leftMostGuardPointSees = (*currentPoints)[vp][0] - 'A';
                if(leftMostGuardPointSees < i){
                    foundPointDontSeeWithOC = true;
                    firstRightViewpointHighOutside = vp;
                }
            }
        }
        rightMostPointGuardSees.push_back(rightMostPointWeSee);
        closestPointHighOutsideRight.push_back(firstRightViewpointHighOutside);
        
        //Check to Left.
        foundPointDontSeeWithOC = false;
        int leftMostPointWeSee = gI, firstLeftViewpointHighOutside = -1;
        for(int vp = gI-1; vp>=0; vp--){
            if((*currentPoints)[vp].size() == 1)
                continue;
            int check = (*currentPoints)[vp].find(gS);

            if(check >= 0){
                if(foundPointDontSeeWithOC){
                    // x(leftMostGuardPointSees) < x(closestPointHighOutsideLeft)
                    return true;
                }
                leftMostPointWeSee = vp;
            }

            else if(!foundPointDontSeeWithOC && cannotBlockWithTerrain[(*currentPoints)[vp] +"," + gS]){
                int rightMostGuardPointSees = (*currentPoints)[vp][(*currentPoints)[vp].size()-1] - 'A';
                if(rightMostGuardPointSees > i){
                    foundPointDontSeeWithOC = true;
                    firstLeftViewpointHighOutside = vp;
                }
            }
        }
        leftMostPointGuardSees.push_back(leftMostPointWeSee);
        closestPointHighOutsideLeft.push_back(firstLeftViewpointHighOutside);
    }
    
    //Higher guard.
    unordered_map<string,string> higherPoint;
    unordered_map<string,string> tooFarAway; 
    
    for(int i=0; i<guardIndices.size(); i++){
        int g1I = guardIndices[i];
        string g1S = (*currentPoints)[g1I];
        for(int j= i+1; j<guardIndices.size(); j++){
            int g2I = guardIndices[j];
            string g2S = (*currentPoints)[g2I];
            string highGuardKey = g1S + "," + g2S;
            higherPoint[highGuardKey] = "unknown";
            tooFarAway[highGuardKey] = "unknown";

            // x(leftMostPointGuardSees(j) < x(i)) AND x(rightMostPointGuardSees[i]) > x(j)
            if(leftMostPointGuardSees[j] <= g1I && rightMostPointGuardSees[i] >= g2I){
                tooFarAway[highGuardKey] = "no";
            }

            // x(closestPointHighOutsideRight(j) <= x(rightMostPointGuardSees(i))
            if(rightMostPointGuardSees[i] >= closestPointHighOutsideRight[j]){
                higherPoint[highGuardKey] = g1S;
            }
            
            // x(leftMostPointGuardSees(j)) < x(closestPointHighOutsideLeft(i))
            if(leftMostPointGuardSees[j] <= closestPointHighOutsideLeft[i]){
                if(higherPoint[highGuardKey] == g1S){
                    return true;
                }
                higherPoint[highGuardKey] = g2S;
            }
            
            // x(closestPointHighOutsideRight(i)) < x(j) AND (i, j) unpiercable
            if(closestPointHighOutsideRight[i] < g2I && cannotBlockWithTerrain[highGuardKey]){
                if(higherPoint[highGuardKey] == g1S){
                    return true;
                }
                tooFarAway[highGuardKey] = "yes"; // g_1 is low outside of disk g_2
                higherPoint[highGuardKey] = g2S;
            }
            
            // x(i) < x(closestPointHighOutsideLeft(j)) AND (i, j) unpiercable
            if(closestPointHighOutsideLeft[j] > g1I && cannotBlockWithTerrain[highGuardKey]){
                if(higherPoint[highGuardKey] == g2S){
                    return true;
                }
                tooFarAway[highGuardKey] = "yes"; // g_1 is high outside of disk g_2
                higherPoint[highGuardKey] = g1S;
            }
        }
    }
    //End Higher Guard

    //If g1 and g2 shouldn't be blocked by terrain, then no ``midguard'' between them can be higher than both of them.
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
                string midGuardS = (*currentPoints)[midGuardI]; // i < k(midguard) < j
                if(higherPoint[g1S + "," + midGuardS] == midGuardS && higherPoint[midGuardS + "," + g2S] == midGuardS){
                    return true;
                }
            }
        }
    }
    //End midGuard higher than both.

    //Simply stating that a point cannot be too far away from itself.
    for (int i = 0; i < (*currentPoints).size()-1; i++)
    {
    	string pS = (*currentPoints)[i];
    	string hashKey = pS + "," + pS;
    	tooFarAway[hashKey] = "no";
    	higherPoint[hashKey] = "unknown";
    }

    
    //Set tooFarAway and highePoint for guard/viewpoint pairs.
    for (int i = 0; i < (*currentPoints).size()-1; i++)
    {
        for (int j = i+1; j < (*currentPoints).size(); j++) // i < j
        {
            // [g, vp] or [vp, g] pairs
            if (((*currentPoints)[i].size() == 1) && ((*currentPoints)[j].size() > 1))
            {
                // i is g and j is vp
                string gS = (*currentPoints)[i];
                string vpS = (*currentPoints)[j];
                int indexWRTGuards = gS[0] - 'A';

                string hashKey = gS + "," + vpS;
                int leftMostGuardPointSees = vpS[0] - 'A';
                int check = vpS.find(gS);
                if (check >= 0) // vp sees g
                {
                    tooFarAway[hashKey] = "no";
                    higherPoint[hashKey] = "unknown";
                }
                else if (rightMostPointGuardSees[indexWRTGuards] >= j && leftMostGuardPointSees <= indexWRTGuards) 
                {
                  //visibilities cross
                  //Note here vp and g cannot be unpiercable because we would have 
                  //rejected above.
                  tooFarAway[hashKey] = "no";
					        higherPoint[hashKey] = "unknown";
                }
                else if (cannotBlockWithTerrain[hashKey])  
                {
					        //They must be too far away.
					        tooFarAway[hashKey] = "yes";
					
                  if(leftMostGuardPointSees <= indexWRTGuards)
                  {
                    //vp sees a guard to the left of g.
                    //Therefore vp is high outside g's disk.
                    higherPoint[hashKey] = vpS;
                  }
                  else if(rightMostPointGuardSees[indexWRTGuards] >= j)
                  {
                    //g sees a point to the right of vp.
                    //Therefore vp is low outside g's disk.
                    higherPoint[hashKey] = gS;                    
                  }
                  else
                  {                    
                    higherPoint[hashKey] = "unknown";
                  }
                    
                }
                else
                {
                 tooFarAway[hashKey] = "unknown";
                 higherPoint[hashKey] = "unknown";
                }
            }
            if (((*currentPoints)[i].size() > 1) && ((*currentPoints)[j].size() == 1))
            {
                // i is vp and j is g
                string vpS = (*currentPoints)[i];
                string gS = (*currentPoints)[j];
                int indexWRTGuards = gS[0] - 'A';

                string hashKey = vpS + "," + gS;
                int rightMostGuardPointSees = vpS[vpS.size()-1] - 'A';
                int check = vpS.find(gS);
                if (check >= 0) // vp sees g
                {
                    tooFarAway[hashKey] = "no";
                    higherPoint[hashKey] = "unknown";
                }
                else if (leftMostPointGuardSees[indexWRTGuards] <= i && rightMostGuardPointSees >= indexWRTGuards) 
                {
                  //visibilities cross
                  //Note here vp and g cannot be unpiercable because we would have 
                  //rejected above.
                  tooFarAway[hashKey] = "no";
					        higherPoint[hashKey] = "unknown";
                }
                else if (cannotBlockWithTerrain[hashKey])  
                {
					        //They must be too far away.
					        tooFarAway[hashKey] = "yes";
					
                  if(rightMostGuardPointSees >= indexWRTGuards)
                  {
                    //vp sees a guard to the right of g.
                    //Therefore vp is high outside g's disk.
                    higherPoint[hashKey] = vpS;
                  }
                  else if(leftMostPointGuardSees[indexWRTGuards] <= i)
                  {
                    //g sees a point to the left of vp.
                    //Therefore vp is low outside g's disk.
                    higherPoint[hashKey] = gS;                    
                  }
                  else
                  {                    
                    higherPoint[hashKey] = "unknown";
                  }
                    
                }
                else
                {
                 tooFarAway[hashKey] = "unknown";
                 higherPoint[hashKey] = "unknown";
                }
            }
        }
    }
    
    
    
    //Check to see if there is a guard that is low outside of a disk but needs to pierce a line segment inside that disk.
    for(int i=0; i<guardIndices.size(); i++){
        int g1I = guardIndices[i];
        string g1S = (*currentPoints)[g1I];
        for(int j= i+1; j<guardIndices.size(); j++){
            int g2I = guardIndices[j];
            string g2S = (*currentPoints)[g2I];
            string hashKey = g1S + "," + g2S;

            //We are looking to see if g1 needs to block g2 from seeing any viewpoints
            //to the left of g1.  If g1 and g2 can be pierced, then g1 doesn't have to be
            //the blocker for g2 for any such viewpoints, so skip this g2.
            if(!cannotBlockWithTerrain[hashKey]) 
                continue;

            for(int vp = g1I-1; vp>leftMostPointGuardSees[j]; vp--){ // x(leftMostPointGuardSees(j)) < x(vp) < x(i)

                //We want to check to see if g2 does not see vp and needs a blocker to pierce
                //\overline{vp g2}.  Since g1 and g2 are unpiercable, it must be that
                //either g1 pierces \overline{vp g2} or that some point to the left of g1 is 
                //the blocker (if g1 does not pierce then nothing between g1 and g2 can pierce).
                string vpS = (*currentPoints)[vp];
                if(vpS.size() == 1) // must be a viewpoint, not guard
                    continue;
                int check = vpS.find(g2S); // vp does not see g2
                if(check >= 0)
                {
                    //If g2 sees vp, then g1 cannot be the blocker for g2 for any vp' to the left of vp, so we can stop checking.
                    break;
                }
                else{
                  string vpG2HashKey = generateHashKey(vp, g2I, currentPoints);
                  if(tooFarAway[vpG2HashKey] != "no")
                  {
                    //We don't know for sure if we need a blocker for this vp, so skip it.
                      continue; 
                  }

                    //If we are here, then g2 does not see vp, and vp is inside g2's disk.
                    //Therefore we must find a blocker in (vp, g1].
                    bool blockerCanBeG1, blockerCanBeNewPoint;


                    //Is there a guard gk such that disc(gk) contains g2 and vp but g1 must 
                    //be low outside?  If so, g1 cannot pierce \overline{g2 vp}.

                    //assuming g1 can be a blocker until we determine otherwise.
                    blockerCanBeG1 = true;
                    for(int k=0; k<guardIndices.size(); k++){
                        if(k == i)
                            continue;
                        int gkI = guardIndices[k];
                        string gkS =  (*currentPoints)[gkI];

                        //If g1 is not low outside gk's disk, skip this gk.
                        string firstHash = generateHashKey(gkI, g1I, currentPoints);              
                        if(tooFarAway[firstHash] != "yes" || higherPoint[firstHash] != gkS)
                            continue;

                        //If g2 is not inside gk's disk, skip this gk.
                        string secondHash = generateHashKey(gkI, g2I, currentPoints);
                        if(tooFarAway[secondHash] != "no")
                            continue;

                        //If we are here, then g1 is low outside gk's disk, and g2 is inside
                        //gk's disk.  If vp is also inside gk's disk, then g1 cannot
                        //be the blocker.
                        string thirdHash = generateHashKey(gkI, vp, currentPoints);
                        if (tooFarAway[thirdHash] == "no")
                        {
                            blockerCanBeG1 = false;
                            break;
                        }
                        else if(gkI > g2I && tooFarAway[thirdHash] == "yes" && higherPoint[thirdHash] == vpS && tooFarAway[firstHash] == "yes" && higherPoint[firstHash] == gkS && tooFarAway[secondHash] == "no"){
                            //vp is high outside gk's disk, and g1 is low outside gk's disk.
                            //Then g1 cannot be a blocker for \overline{vp g2}.
                            blockerCanBeG1 = false;
                            break;
                        }
                    }
                    if(blockerCanBeG1){
                        //if g1 can be the blocker then we move on to the next vp.
                        continue;
                    }

                    //If we are here, then g1 could not be the blocker, so the terrain will
                    // need to have a "new point" that is not in the point order to serve as 
                    //the blocker for \overline{vp g2}.  There are 2 problems this could cause:
                    if(cannotBlockWithTerrain[vpS + "," + g1S]){
                        //The new blocker would have to block g1 from vp as well.  So if vp and
                        //g1 are unpiercable, we can reject the order.
                        blockerCanBeNewPoint = false;
                    }
                    else{
                        //New blocker may make more things unpiercable.
                        // In particular, every viewpoint vp2 to the left of vp such that 
                        //vp2 and g1 are unpiercable, we would now have that vp2 and g2 are 
                        //unpiercable whereas they might have been able to have been blocked 
                        //before.  If vp2 would need to be high and outside of g2's disk then 
                        //we may be able to reject.
                        blockerCanBeNewPoint = true;
                        for(int vp2 = vp-1; vp2 > leftMostPointGuardSees[j]; vp2--){
                            check = (*currentPoints)[vp2].find(g1S);
                            if(check >=0){
                                //g1 sees vp2.
                                check = (*currentPoints)[vp2].find(g2S);
                                if(check < 0){
                                    //g2 does not see vp2, but now it is unpiercable and it sees
                                    //a point to the left of vp2 (we stop checking vp2s when we get to the last point g2 sees).

                                    //It could only be low outside g2's disk.  If vp2 sees a guard to the right of g2, the order is not realizable.
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
                        return true;
                }
            }
            

            //Repeat the process except now we are checking on blockers for \overline{g1 vp} for
            //vps to the right of g2.
            for(int vp = g2I+1; vp<rightMostPointGuardSees[i]; vp++){
                string vpS = (*currentPoints)[vp];
                if(vpS.size() == 1)
                    continue;
              
                int check = vpS.find(g1S);
                if(check >= 0)
                {
                  //If vp sees g1 then g2 will not be a blocker for any point right of vp.
                    break;
                }
                else{

                  string vpG1HashKey = generateHashKey(g1I, vp, currentPoints);
                  if(tooFarAway[vpG1HashKey] != "no") // vp sees a guard right of g2
                  {
                    //We don't know for sure if we need a blocker for this vp, so skip it.
                      continue; // check both side
                  }

                    bool blockerCanBeG2, blockerCanBeNewPoint;
                    //Is there a disc that contains g1 and vp but g2?
                    blockerCanBeG2 = true;
                    for(int k=0; k<guardIndices.size(); k++){
                        if(k == j)
                            continue;
                        int gkI = guardIndices[k];
                        string gkS = (*currentPoints)[gkI];
                        string firstHash = generateHashKey(gkI, g1I, currentPoints);
                        if(tooFarAway[firstHash] != "no")
                            continue;
                        string secondHash = generateHashKey(gkI, g2I, currentPoints);
                        if(tooFarAway[secondHash] != "yes" || higherPoint[secondHash] != gkS)
                            continue;
                        string thirdHash = generateHashKey(gkI, vp, currentPoints);
                        if(tooFarAway[thirdHash] == "no")
                        {
                            blockerCanBeG2 = false;
                            break;
                        }
                        else if(gkI < g1I && tooFarAway[thirdHash] == "yes" && higherPoint[thirdHash] == vpS && tooFarAway[secondHash] == "yes" && higherPoint[secondHash] == gkS && tooFarAway[firstHash] == "no"){
                            blockerCanBeG2 = false;
                            break;
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
                        return true;
                }
            }
        }
    }
    


    //We now check for violations of Lemma 5 where the "outer" points of the lemma are guards g1 and g2 and the "inner" points are viewpoints.
    for(int i=0; i<guardIndices.size(); i++){
        int g1I = guardIndices[i];
        string g1S = (*currentPoints)[g1I];
        for(int j= i+1; j<guardIndices.size(); j++){
            int g2I = guardIndices[j];
            string g2S = (*currentPoints)[g2I];

            //We now look for a point vp2 that sees g2 but is low outside of g1's disk.
            for(int vp2 = g1I + 1; vp2 < g2I; vp2++){

               string vp2S = (*currentPoints)[vp2];

               int check = vp2S.find(g1S);
               if(check >= 0) // Skip if vp2 sees g1.
                    continue;

               check = vp2S.find(g2S);
               if(check < 0) // Skip if vp2 doesn't see g2.
                    continue;

               string hashKey2 = g1S + "," + vp2S;

               if(tooFarAway[hashKey2] != "yes" || higherPoint[hashKey2] != g1S) 
                    continue;
               

               //So now we have that vp2 is low outside of g1.
               //If there is a vp1 between vp2 and g2 that sees g1, but has to be low 
               //outside g2's disk, the order is unrealizable.
               for(int vp1 = vp2 +1; vp1 < g2I; vp1++) {

                    string vp1S = (*currentPoints)[vp1];

                    check = vp1S.find(g1S);
                    if(check < 0) // Skip if vp1 doesn't see g1.
                        continue;

                    check = vp1S.find(g2S);
                    if(check >= 0) // Skip if vp1 sees g2.
                        continue;

                    string hashKey1 = vp1S + "," + g2S;
                    if(tooFarAway[hashKey1] != "yes" || higherPoint[hashKey1] != g2S) 
                        continue;
                   
                    //If we make it here then g1, vp2, vp1, g2 violate Lemma 5.
                    return true;
               }
            }
        }
    }
    output.close();

    //If we make it here, we could not determine that the order is unrealizable.
    return false;
}

string generateHashKey(int index1, int index2, vector<string> *arr)
{
    string hashKey;
    if (index1 <= index2)
    {
        hashKey = (*arr)[index1] + "," + (*arr)[index2];
    }
    else
    {
        hashKey = (*arr)[index2] + "," + (*arr)[index1];
    }
    return hashKey;
}