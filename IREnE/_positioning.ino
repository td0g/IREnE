boolean calcPointFromTargetLines(){//   POSITION CALCULATIONS   ############################################################################################################################################################
  switch (targetCount){
    case 0: return false;
    case 1:
      IREnE.objectX(IREnE.abTOx(targets[0][0],targets[0][1]));
      IREnE.objectY(IREnE.abTOy(targets[0][0],targets[0][1]));
      #ifdef DEBUG
        Serial.print(F("SINGLE POINT x,y "));
        Serial.print(IREnE.objectX());
        Serial.print(" ");
        Serial.println(IREnE.objectY());
      #endif
    break;
    case 2:
      IREnE.abcPairIntersection(targets[0][0],targets[1][0],targets[0][1],targets[1][1],targets[0][2],targets[1][2]);
      #ifdef DEBUG
        Serial.print(F("INTERSECTION x,y "));
        Serial.print(IREnE.objectX());
        Serial.print(" ");
        Serial.println(IREnE.objectY());
      #endif
    break;
  }
  return true;
}


void calcCurrentxy(){
  x = IREnE.abTOx(a.currentPosition(), b.currentPosition());
  y = IREnE.abTOy(a.currentPosition(), b.currentPosition());
  theta = IREnE.acTOtheta(a.currentPosition(), c.currentPosition());
}


void calcCurrDistToObject(){
  if (targetCount){
    calcCurrentxy();
    float f0 = x - IREnE.objectX();
    f0 = fabs(f0);
    f0 *= f0;
    float f1 =y - IREnE.objectY();
    f1 = fabs(f1);
    f1 *= f1;
    f0 += f1;
    f0 = sqrt(f0);
    distO = f0;
    #ifdef DEBUG
      Serial.println(F("  COMPUTING DIST TO OBJ"));
      Serial.print("Distance: ");
      Serial.println(distO);
    #endif
  
    float dY = y;
    dY -= IREnE.objectY();
    float dX = x;
    dX -= IREnE.objectX();
    thetaO = dX / dY;
    thetaO = atan(thetaO);
//NEED TO ADD A WATCH HERE 0.5.4
    //Add pi WHEN??
    if (IREnE.objectY() < y && IREnE.objectX() > x) {
      #ifdef DEBUG
        Serial.print(F("Theta: "));
        Serial.println(thetaO);
        Serial.println(F("Added PI"));
      #endif
      thetaO += PI;
    }
    else if (IREnE.objectY() < y && IREnE.objectX() < x){
      #ifdef DEBUG
        Serial.print(F("Theta: "));
        Serial.println(thetaO);
        Serial.println(F("Subtracted PI"));
      #endif
      thetaO -= PI;
      
    }
    #ifdef DEBUG
      Serial.print(F("Theta: "));
      Serial.println(thetaO);
      Serial.print(F("x, y, oX, oY:"));
      Serial.print(x);
      Serial.print(" ");
      Serial.print(y);
      Serial.print(" ");
      Serial.print(IREnE.objectX());
      Serial.print(" ");
      Serial.println(IREnE.objectY());
    #endif
  
    f0 = IREnE.objectX();
    f0 *= IREnE.objectX();
    f1 = IREnE.objectY();
    f1 *= IREnE.objectY();
    f0 += f1;
    distOMax = sqrt(f0);
    distOMax -= bMin * B;
  }
  else distO = 0;
}

void moveToxytheta(){
  a.moveTo(IREnE.xyTOa(xT, yT));
  bMove(IREnE.xyTOb(xT, yT));
  c.moveTo(cRot(IREnE.xythetaTOc(xT, yT, thetaT)));

  #ifdef DEBUG
    Serial.println(F("  COMPUTING AB FROM xT,yT, thetaT"));
    Serial.print(F("xT:"));
    Serial.print(xT);
    Serial.print(F(" yT:"));
    Serial.print(yT);
    Serial.print(F(" thetaT:"));
    Serial.println(thetaT);
    Serial.print(F("a: "));
    Serial.print(a.targetPosition());
    Serial.print(F(" b: "));
    Serial.print(b.targetPosition());
    Serial.print(F(" c: "));
    Serial.println(c.targetPosition());
    Serial.println();
  #endif
}

void moveToObjThetaDist(){
  xT = IREnE.objThetaDistTOx(thetaT, distT);
  yT = IREnE.objThetaDistTOy(thetaT, distT);
  
  #ifdef DEBUG
    Serial.println(F("   moveToObjThetaDist"));
    Serial.print(F("oX, oY: "));
    Serial.print(IREnE.objectX());
    Serial.print(F(", "));
    Serial.println(IREnE.objectY());
    Serial.print(F("distT: "));
    Serial.print(distT);
    Serial.print(F(" thetaT: "));
    Serial.println(thetaT);
    Serial.print(F("xT, yT: "));
    Serial.print(xT);
    Serial.print(F(", "));
    Serial.println(yT);
    Serial.println();
  #endif
}
