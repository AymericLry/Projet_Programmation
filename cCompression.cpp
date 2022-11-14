#include "cCompression.h"
#include <cmath>
#include <iostream>

using namespace std;

cCompression::cCompression(unsigned int largeur, unsigned int hauteur, int Bloc8x8[][taille], unsigned int qualite)
{
    mLargeur = largeur;
    mHauteur = hauteur;
    mQualite = qualite;

    mBuffer = new unsigned int*[taille];
    for(unsigned i=0; i<taille; i++)
    {
        mBuffer[i] = new unsigned int[taille];
    }

    for(unsigned i=0; i<taille; i++)
    {
        for(unsigned j=0; j<taille; j++)
        {
            mBuffer[i][j] = Bloc8x8[i][j];
        }
    }
    //ctor
}

cCompression::~cCompression()
{
    //dtor
}

unsigned int cCompression::GetLageur() const
{
    return mLargeur;
}

unsigned int cCompression::GetHauteur() const
{
    return mHauteur;
}

unsigned int cCompression::GetQualite() const
{
    return mQualite;
}

void cCompression::SetLageur(unsigned int L)
{
    mLargeur = L;
}

void cCompression::SetHauteur(unsigned int H)
{
    mHauteur = H;
}

void cCompression::SetQualite(unsigned int Q)
{
    mQualite = Q;
}

void cCompression::Calcul_DCT_Block(double DCT_Img[][taille], int Bloc8x8[][taille])
{
    unsigned x, y, u, v;
    double C[8];
    C[0] = 1./sqrt(2);
    for(unsigned n=1; n<8; n++)
    {
        C[n] = 1.;
    }
    double somme = 0.;

    for(u=0; u<8; u++)
    {
        for(v=0; v<8; v++)
        {
            for(x=0; x<8; x++)
            {
                for(y=0; y<8; y++)
                {
                    somme += (Bloc8x8[x][y]-128) * cos( (2*x+1)*M_PI*u / 16 ) * cos( (2*y+1)*M_PI*v / 16 ); //img-128 pour ramener entre -128 et 127
                }//fin for(y)
            }//fin for(x)
            DCT_Img[u][v] = round( (1./4) * C[u] * C[v] * somme * 100)/100;
            //DCT_Img[u][v] = (1./4) * C[u] * C[v] * somme;
            somme = 0.;
        }//fin for(v)
    }//fin for(u)
}

void cCompression::Calcul_IDCT(double DCT_Img[][taille], int Bloc8x8[][taille])
{
    unsigned x, y, u, v;
    double C[8];
    C[0] = 1./sqrt(2);
    for(unsigned n=1; n<8; n++)
    {
        C[n] = 1.;
    }
    double somme = 0.;

    for(x=0; x<8; x++)
    {
        for(y=0; y<8; y++)
        {
            for(u=0; u<8; u++)
            {
                for(v=0; v<8; v++)
                {
                    somme += DCT_Img[u][v] * C[u] * C[v] * cos( (2*x+1)*M_PI*u / 16 ) * cos( (2*y+1)*M_PI*v / 16 );
                }//fin for(v)
            }//fin for(u)
            Bloc8x8[x][y] = round(somme / 4) + 128;
            somme = 0.;
        }//fin for(y)
    }//fin for(x)
}

void cCompression::creationQtab(int Qtab[][taille])
{
    double lbd;
    double cond;

    if(mQualite < 50)
    {
        lbd = 5000. / mQualite;
    }
    else
    {
        lbd = 200 - 2*mQualite;
    }

    for(unsigned i=0; i<taille; i++)
    {
        for(unsigned j=0; j<taille; j++)
        {
            cond = (Q[i][j]*lbd + 50) / 100;
            if(cond < 1)
            {
                Qtab[i][j] = 1;
            }
            else if(cond > 255)
            {
                Qtab[i][j] = 255;
            }
            else
            {
                Qtab[i][j] = cond;
            }
        }
    }
}


void cCompression::quant_JPEG(double Img_DCT[][taille], int Img_Quant[][taille])
{
    int Qtab[taille][taille];
    creationQtab(Qtab);

    for(unsigned u=0; u<taille; u++)
    {
        for(unsigned v=0; v<taille; v++)
        {
            Img_Quant[u][v] = round( Img_DCT[u][v] / Qtab[u][v] );
        }
    }
}

void cCompression::dequant_JPEG(double Img_DCT[][taille], int Img_Quant[][taille])
{
    int Qtab[taille][taille];
    creationQtab(Qtab);

    for(unsigned u=0; u<taille; u++)
    {
        for(unsigned v=0; v<taille; v++)
        {
            Img_DCT[u][v] = round( Img_Quant[u][v] * Qtab[u][v] );
        }
    }
}

double cCompression::EQM(int Img_IDCT[][taille])
{
    double somme = 0.;
    double EQM = 0.;

    for(unsigned i=0; i<taille; i++)
    {
        for(unsigned j=0; j<taille; j++)
        {
            somme += (mBuffer[i][j] - Img_IDCT[i][j]) * (mBuffer[i][j] - Img_IDCT[i][j]);
        }
    }
    EQM = round(somme / (taille*taille));
    return EQM;
}

double cCompression::taux_compression(double Bloc8x8[][taille])
{
    double taux = 0.;
    int nbinit = 0;
    int nbcomp = 0;
    for(unsigned i=0; i<taille; i++)
    {
        for(unsigned j=0; j<taille; j++)
        {
            if(mBuffer[i][j] != 0)
            {
                nbinit++;
            }
            if(Bloc8x8[i][j] != 0)
            {
                nbcomp++;
            }
        }
    }
    taux = round( 100-((100*nbcomp)*1./nbinit) );
    return taux;
}

/*void cCompression::RLE\_Block(int Bloc8x8[][taille], int DC_precedent, char *Trame)
{


}*/

void cCompression::Testalgo()
{
    int im = 0;
    int repet = 1;  //nombre de répétition

    for(int nbi=2; nbi < taille+1; nbi+=2)
    {
        // Fonctionnement normal
        for(int i=0; i<=nbi; i++)
        {
            if(i>=taille)
            {
                cout << "i = " << taille-1 << endl;
            }
            else
            {
                cout << "i = " << i << endl;
            }
        }
        if (repet<(taille/2))
        {
            for(int i=nbi-1; i>=0; i--)
            {
                cout << "i = " << i << endl;
            }
        }

        cout << endl;

        for(int j=(nbi-1); j>=0; j--)
        {
            cout << "j = " << j << endl;
        }
        if (repet<(taille/2))
        {
            for(int j=0; j<=(nbi+1); j++)
            {
                cout << "j = " << j << endl;
            }
        }

        // Fonctionnement final







        repet++;
        cout << repet-1 << endl << endl;
    }

}


