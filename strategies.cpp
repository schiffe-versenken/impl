#include "strategies.h"
#include <random>
#include <map>
#include <functional>

std::function<void(StrategyBlock*, BlockCoordinate)> generator;

void testStrategy(StrategyBlock* b, BlockCoordinate c)
{
	for (int i = 0; i < b->size(); ++i) {
		(*b)[i] = i + 1;
	}
}

void randomStrategy(StrategyBlock* b, BlockCoordinate c)
{
	std::uniform_int_distribution<> dis(1, CELLS);
	for (int i = 0; i < BLOCK_SIZE; ++i) {
		(*b)[i] = dis(GENERATOR);
	}
}

//Strategy* fullGridStrategy()
//{
//	Strategy* strat = emptyStrategy();
//	int L=0;
//	std::vector<int> K (1,(int)floor((double)N/2));
//	int maxCoord = K[0];
//    std::vector<int> lastShot (D,maxCoord);
//    std::vector<int> lastIndeces (D,0);
//    (*strat)[0]=toIndex(lastShot);
//
//	for (int i = 1; i < CELLS; ++i) {
//        //Adding new Coords if needed
//        if(lastShot ==  std::vector<int> (D,maxCoord)){
//            int min = (int)ceil((double)K[(int)pow(2.0,L)-1]/2);
//            //Here pow(2.0,L+1)-1 instead of pow(2.0,L+1)-2, because last element of iterator is not added so we have to reference one element further
//            std::vector<int> K_Old (K[(int)pow(2.0,L)-1],K[(int)pow(2.0,L+1)-1]);
//            for(int k:K_Old){
//                K.emplace_back(k-min);
//                K.emplace_back(k+min);
//            }
//            L++;
//            maxCoord = K[(int)pow(2.0,L+1)-2];
//        }
//
//        //calculating next Shot
//        int count = D-1;
//        while(true){
//            if(lastShot[count]==maxCoord){
//                lastShot[count]=K[0];
//                lastIndeces[count]=0;
//                count--;
//            }else{
//                lastShot[count]=K[lastIndeces[count]+1];
//                lastIndeces[count]++;
//                break;
//            }
//        }
//        (*strat)[i]=toIndex(lastShot);
//	}
//	return strat;
//}
//
//Strategy* sparseGridStrategy(){
//    Strategy* strat = emptyStrategy();
//	int L=0;
//	int countCoord=0;
//	int position=0;
//	std::vector<std::vector<int> > K;
//	std::vector<int> K0 (1,(int)floor((double)N/2));
//	K.push_back(K0);
//	int maxCoord = K[0][0];
//    std::vector<Tuple> lastShot;
//    Tuple t1;
//    t1.coords = std::vector<int> (D,maxCoord);
//    t1.levels = std::vector<int> (D,0);
//    lastShot.push_back(t1);
//    (*strat)[0]=toIndex(lastShot[0].coords);
//
//    for (int i = 1; i < CELLS; ++i) {
//        std::vector<int> vgl (D,K[0][0]);
//        vgl.back()=maxCoord;
//        if(lastShot.back().coords==vgl){
//            int min = (int) ceil((double)K[L][0]/2);
//            std::vector<int> KAdd;
//            K.push_back(KAdd);
//            for(int k : K[L]){
//                K[L+1].push_back(k-min);
//                K[L+1].push_back(k+min);
//            }
//            L++;
//            maxCoord = K[L].back();
//            countCoord = 0;
//            position = 0;
//        }
//
//        Tuple out = lastShot[0];
//        if(countCoord == 0){
//            out.levels[position]++;
//        }
//        out.coords[position] = K[out.levels[position]][countCoord];
//        countCoord++;
//        if(countCoord == K[out.levels[position]].size()){
//            countCoord = 0;
//            position++;
//            if(position == D){
//                position = 0;
//                lastShot.erase(lastShot.begin());
//            }
//        }
//        lastShot.push_back(out);
//        (*strat)[i]=toIndex(out.coords);
//    }
//}

std::map<std::string, std::function<void (StrategyBlock*, BlockCoordinate)>> strategyNames = {
	{ "test", &testStrategy },
	{ "random", &randomStrategy } /*
	{ "fullGrid", &fullGridStrategy },
	{ "sparseGrid", &sparseGridStrategy}*/
};

void createStrategy(std::string& name)
{
	generator = strategyNames[name];
}

BlockCoordinate generateBlock(StrategyBlock* b, int index)
{
	BlockCoordinate coord = emptyBlockCoord();
	for (int d = BLOCK_DIMENSION - 1; d >= 0; --d) {
		coord[d] = index / std::pow((double)N, d);
		index = index % (int) std::pow((double)N, d);
	}

	generator(b, coord);
	return coord;
}

int traverse(StrategyBlock& strat, Coordinate& c, Coordinate& max, int d)
{
	int length = max[d] - c[d] + 1;
	int min = CELLS;
	if (d == D - 1)
	{
		int index = toIndex(c);
		for (int i = 0; i < length; ++i) {
			int value = strat[index + i];
			if (value < min) {
				min = value;
			}
		}
	}
	else {
		Coordinate cnew = c;
		for (int i = 0; i < length; ++i) {
			int value = traverse(strat, cnew, max, d + 1);
			if (value < min) {
				min = value;
			}
			cnew[d] = cnew[d] + 1;
		}
	}
	return min;

}

bool inBlock(BlockCoordinate& c, Ship& s)
{
	for (int d = 0; d < BLOCK_DIMENSION; ++d) {
		if ((s.min[d] < c[d] && s.max[d] < c[d]) || (s.min[d] > c[d] && s.max[d] > c[d]))
		{
			return false;
		}
	}
	return true;
}

int findMin(BlockCoordinate& c, StrategyBlock& strat, Ship& s)
{
	if (inBlock(c, s))
	{
		Coordinate min = s.min, max = s.max;
		for (int d = 0; d < BLOCK_DIMENSION; ++d) {
			min[d] = c[d];
			max[d] = c[d];
		}
		return traverse(strat, min, max, BLOCK_DIMENSION);
	}
	return -1;
}
