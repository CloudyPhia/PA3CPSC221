
#include "stats.h"

stats::stats(PNG & im){

/* your code here! */
  HSLAPixel* currPixel;
  sumSat.clear();
  sumSat.resize(im.width(), vector<double>(im.height(), 0));
  sumLum.clear();
  sumLum.resize(im.width(), vector<double>(im.height(), 0));
  sumHueX.clear();
  sumHueX.resize(im.width(), vector<double>(im.height(), 0));
  sumHueY.clear();
  sumHueY.resize(im.width(), vector<double>(im.height(), 0));
  hist.clear();
  hist.resize(im.width(), vector<vector<int>>(im.height(), vector<int>(36,0)));

  for (int i = 0; i < (int) im.height(); i++) {
    for (int j = 0; j < (int) im.width(); j++) {

      currPixel = im.getPixel(j, i);

      if (i == 0 && j == 0) {
        sumSat[j][i] = currPixel->s;
        sumLum[j][i] = currPixel->l;
        sumHueX[j][i] = cos((M_PI * currPixel->h) / 180);
        sumHueY[j][i] = sin((M_PI * currPixel->h) / 180);
        if (floor(currPixel->h / 10) == 36) {
          hist[j][i][0]++;
        } else {
          hist[j][i][floor(currPixel->h / 10)]++;
        }
      } else if (j == 0) {
        sumSat[j][i] = sumSat[j][i - 1] + currPixel->s;
        sumLum[j][i] = sumLum[j][i - 1] + currPixel->l;
        sumHueX[j][i] = sumHueX[j][i - 1] + cos((M_PI * currPixel->h) / 180);
        sumHueY[j][i] = sumHueY[j][i - 1] + sin((M_PI * currPixel->h) / 180);
        createHistogram(j, i);
        if (floor(currPixel->h / 10) == 36) {
          hist[j][i][0]++;
        } else {
          hist[j][i][floor(currPixel->h / 10)]++;
        }
      } else if (i == 0) {
        sumSat[j][i] = sumSat[j - 1][i] + currPixel->s;
        sumLum[j][i] = sumLum[j - 1][i] + currPixel->l;
        sumHueX[j][i] = sumHueX[j - 1][i] + cos((M_PI * currPixel->h) / 180);
        sumHueY[j][i] = sumHueY[j - 1][i] + sin((M_PI * currPixel->h) / 180);
        createHistogram(j, i);
        if (floor(currPixel->h / 10) == 36) {
          hist[j][i][0]++;
        } else {
          hist[j][i][floor(currPixel->h / 10)]++;
        }

      } else {
        sumSat[j][i] = sumSat[j - 1][i] + sumSat[j][i - 1] - sumSat[j - 1][i - 1] + currPixel->s;
        sumLum[j][i] = sumLum[j - 1][i] + sumLum[j][i - 1] - sumLum[j - 1][i - 1] + currPixel->l;
        sumHueX[j][i] = sumHueX[j - 1][i] + sumHueX[j][i - 1] - sumHueX[j-1][i - 1] + cos((M_PI * currPixel->h) / 180);
        sumHueY[j][i] = sumHueY[j - 1][i] + sumHueY[j][i - 1] - sumHueY[j-1][i - 1] + sin((M_PI * currPixel->h) / 180);
        createHistogram(j, i);
        if (floor(currPixel->h / 10) == 36) {
          hist[j][i][0]++;
        } else {
          hist[j][i][floor(currPixel->h / 10)]++;
        }
      }
    }
  }
}

void stats::createHistogram(int x, int y) {
  int cutOff = 36;
  for (int k = 0; k < cutOff; k++) {
    if (x == 0) {
      hist[x][y][k] = hist[x][y - 1][k];
    } else if (y == 0) {
      hist[x][y][k] = hist[x - 1][y][k];
    } else {
      hist[x][y][k] = hist[x - 1][y][k] + hist[x][y - 1][k]-hist[x - 1][y - 1][k];
    }
  }
}


long stats::rectArea(pair<int,int> ul, pair<int,int> lr){

/* your code here */
    int area = ((lr.first - ul.first) + 1) * ((lr.second - ul.second) + 1);

    return area;

}

HSLAPixel stats::getAvg(pair<int,int> ul, pair<int,int> lr){

/* your code here */
long numPixel = rectArea(ul, lr);
  int xTop = ul.first;
  int yTop = ul.second;
  int xBtm = lr.first;
  int yBtm = lr.second;
  HSLAPixel avgPixel;
  double avgHue, avgSat, avgLum;

  if (xTop == 0 && yTop == 0) {
    avgHue = atan2((sumHueY[xBtm][yBtm]),(sumHueX[xBtm][yBtm])) * 180 / M_PI;
    avgSat = sumSat[xBtm][yBtm] / numPixel;
    avgLum = sumLum[xBtm][yBtm] / numPixel;
  } else if (xTop == 0) {
    avgHue = atan2((sumHueY[xBtm][yBtm] - sumHueY[xBtm][yTop - 1]),
                        (sumHueX[xBtm][yBtm] - sumHueX[xBtm][yTop - 1])) * 180 / M_PI;
    avgSat = (sumSat[xBtm][yBtm] - sumSat[xBtm][yTop - 1]) / numPixel;
    avgLum = (sumLum[xBtm][yBtm] - sumLum[xBtm][yTop - 1]) / numPixel;
  } else if (yTop == 0) {
    avgHue = atan2((sumHueY[xBtm][yBtm] - sumHueY[xTop - 1][yBtm]),
                        (sumHueX[xBtm][yBtm] - sumHueX[xTop - 1][yBtm])) * 180 / M_PI;
    avgSat = (sumSat[xBtm][yBtm] - sumSat[xTop - 1][yBtm]) / numPixel;
    avgLum = (sumLum[xBtm][yBtm] - sumLum[xTop - 1][yBtm]) / numPixel;
  } else {

  avgHue = atan2((sumHueY[xBtm][yBtm] - sumHueY[xTop - 1][yBtm] - sumHueY[xBtm][yTop - 1] + sumHueY[xTop - 1][yTop - 1]),
                        (sumHueX[xBtm][yBtm] - sumHueX[xTop - 1][yBtm] - sumHueX[xBtm][yTop - 1] + sumHueX[xTop - 1][yTop - 1])) * 180 / M_PI;
  avgSat = (sumSat[xBtm][yBtm] - sumSat[xTop - 1][yBtm] - sumSat[xBtm][yTop - 1] + sumSat[xTop - 1][yTop - 1]) / numPixel;
  avgLum = (sumLum[xBtm][yBtm] - sumLum[xTop - 1][yBtm] - sumLum[xBtm][yTop - 1] + sumLum[xTop - 1][yTop - 1]) / numPixel;

}

return avgPixel = HSLAPixel(avgHue, avgSat, avgLum);
}

double stats::entropy(pair<int,int> ul, pair<int,int> lr){

    vector<int> distn;

    /* using private member hist, assemble the distribution over the
    *  given rectangle defined by points ul, and lr into variable distn.
    *  You will use distn to compute the entropy over the rectangle.
    *  if any bin in the distn has frequency 0, then do not add that
    *  term to the entropy total. see .h file for more details.
    */

    /* my code includes the following lines:
        if (distn[i] > 0 )
            entropy += ((double) distn[i]/(double) area)
                                    * log2((double) distn[i]/(double) area);
    */
    distn.resize(36);
    int xTop = ul.first;
    int yTop = ul.second;
    int xBtm = lr.first;
    int yBtm = lr.second;
    int cutOff = 36;
    double entropy = 0.0;
    double area = (double) rectArea(ul,lr);

    if (xTop == 0 && yTop == 0) {
      for(int k = 0; k < cutOff; k++) {
        distn[k] = hist[xBtm][yBtm][k];
      }
    } else if (xTop == 0) {
      for (int k = 0; k < cutOff; k++) {
        distn[k] = hist[xBtm][yBtm][k] - hist[xBtm][yTop - 1][k];
      }
    } else if (yTop == 0) {
      for (int k = 0; k < cutOff; k++) {
        distn[k] = hist[xBtm][yBtm][k] - hist[xTop - 1][yBtm][k];
      }
    } else {
      for (int k = 0; k < cutOff; k++) {
          distn[k] = hist[xBtm][yBtm][k] - hist[xTop - 1][yBtm][k] - hist[xBtm][yTop - 1][k] + hist[xTop - 1][yTop - 1][k];
      }
    }

    for (int i = 0; i < cutOff; i++) {
      if (distn[i] > 0)
          entropy += ((double) distn[i]/(double) area)
                                  * log2((double) distn[i]/(double) area);
    }
    return  -1 * entropy;
}
