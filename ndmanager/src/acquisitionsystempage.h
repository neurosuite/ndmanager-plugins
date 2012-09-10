/***************************************************************************
 *   Copyright (C) 2004 by Lynn Hazan                                      *
 *   lynn.hazan@myrealbox.com                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef ACQUISITIONSYSTEMPAGE_H
#define ACQUISITIONSYSTEMPAGE_H

//include files for the application
#include <acquisitionsystemlayout.h>
#include <helper.h>

// include files for QT
#include <qwidget.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qvalidator.h>




/**
@author Lynn Hazan
*/
class AcquisitionSystemPage : public AcquisitionSystemLayout
{
Q_OBJECT
public:
    AcquisitionSystemPage(QWidget* parent = 0);

    ~AcquisitionSystemPage();
    
  /**Sets the number of channels.*/
  inline void setNbChannels(int nb){
   nbChannels = nb;
   nbChannelsLineEdit->setText(QString::fromLatin1("%1").arg(nb));
  }

  /**Sets the sampling rate.*/
  inline void setSamplingRate(double rate){ 
  samplingRateLineEdit->setText(Helper::doubleToString(rate));
  }

  /**Sets the resolution of the acquisition system.*/
   inline void setResolution(int res){
    switch(res){
       case 12:
         resolutionComboBox->setCurrentItem(0);
         break;
       case 14:
         resolutionComboBox->setCurrentItem(1);
         break;
       case 16:
         resolutionComboBox->setCurrentItem(2);
         break;
       case 32:
         resolutionComboBox->setCurrentItem(3);
         break;
       default:
         resolutionComboBox->setCurrentItem(2);
         break;
    }
   }
   
  /**Sets the initial offset for all the field potentials.*/
  inline void setOffset(int offset){offsetLineEdit->setText(QString::fromLatin1("%1").arg(offset));}

  /**Sets the voltage range of the acquisition system in milivolts.
  */
  inline void setVoltageRange(int value){
   voltageRangeLineEdit->setText(QString::fromLatin1("%1").arg(value));
  }

  /**Sets the amplification of the acquisition system.
  */
  inline void setAmplification(int value){
   amplificationLineEdit->setText(QString::fromLatin1("%1").arg(value));
  }

  /**Returns the number of channels.*/
  inline int getNbChannels() const{return nbChannelsLineEdit->text().toInt();}

  /**Returns the sampling rate.*/
  inline double getSamplingRate() const{return samplingRateLineEdit->text().toDouble();}
 
  /**Returns the resolution of the acquisition system.*/
  inline int getResolution()const{
   switch(resolutionComboBox->currentItem()){
      case 0:
        return 12;
      case 1:
        return 14;
      case 2:
        return 16;
      case 3:
        return 32;
      default:
        return 16;
   }    
  }
  
  /**Returns the initial offset for all the field potentials.*/
  inline int getOffset() const{return offsetLineEdit->text().toInt();}
  
  /**Returns the voltage range of the acquisition system in volts.
  */
  inline int getVoltageRange() const{
   return voltageRangeLineEdit->text().toInt();
  }

  /**Returns the amplification of the acquisition system.
  */
  inline int getAmplification() const{
   return amplificationLineEdit->text().toInt();
  }
 
  /**True if at least one property has been modified, false otherwise.*/
  inline bool isModified()const{return modified;}
 
  /**Checks if the number of channels has changed.*/
  void checkNbChannels();
   
  public slots:
   /**This function is called when the user presses the return key in the <i>Number of Channels</i> field.*/
   void nbChannelsLineEditReturnPressed();
   
   /**This function is called when the <i>Number of Channels</i> field lose focus.*/
   void nbChannelsLineEditLostFocus(); 
  
   /** Will be called when any properties except the number of channels has been modified.*/
  inline void propertyModified(){if(!isInit) modified = true;}

  /**Indicates that the initialisation is finished.*/
  inline void initialisationOver(){isInit = false;}
    
  /**Resets the internal modification status to false.*/
  inline void resetModificationStatus(){modified = false;}
   
  signals:
   /*Signals that the number of channels has been modified.
   * @param nbChannels the neew number of channels
   */
   void nbChannelsModified(int nbChannels);
   
  private:
  
  /**This function is called when the user changes the number of channels. It warns him of the consequences and triggers the implied changes.*/
  void nbChannelsChanged(); 
    
  QDoubleValidator doubleValidator;
  QIntValidator intValidator;
  bool isInit;
  float nbChannels;
  bool reset;
  bool isLostFocus;
  bool isReturnPressed;
  bool modified;
  bool isCheckAsked;
};

#endif
