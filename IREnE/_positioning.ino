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
    unsigned long t0 = abs(x - IREnE.objectX());
    unsigned long t1 = abs(y - IREnE.objectY());
    if (t0 < 32700 && t1 < 32700){
      t0 *= t0;
      t1 *= t1;
      t0 += t1;
      t0 = sqrt(t0);
      distO = t0;
    }
    else {
      float f0 = t0;
      f0 *= f0;
      float f1 = t1;
      f1 *= f1;
      f0 += f1;
      f0 = sqrt(f0);
      distO = f0;
    }
    #ifdef DEBUG
      Serial.println(F("  COMPUTING DIST TO OBJ"));
      Serial.print("Distance: ");
      Serial.println(distO);
    #endif
  
    long dY = y;
    dY -= IREnE.objectY();
    long dX = x;
    dX -= IREnE.objectX();
    thetaO = dX;
    thetaO /= dY;
    thetaO = atan(thetaO);
    #ifdef DEUBG
      Serial.print("Theta: ");
      Serial.println(thetaO);
    #endif
  
    float ft0 = IREnE.objectX();
    ft0 *= IREnE.objectX();
    float ft1 = IREnE.objectY();
    ft1 *= IREnE.objectY();
    ft0 += ft1;
    distOMax = sqrt(ft0);
    distOMax -= bMin * B;
  }
  else {
    distO = 0;
  }
  #ifdef DEBUG
    Serial.print("Max Distance: ");
    Serial.println(distOMax);
    Serial.println(F("  DIST COMPUTED"));
  #endif
  
}

void moveToxytheta(){
  a.moveTo(IREnE.xyTOa(xT, yT));
  b.moveTo(min(bMax, IREnE.xyTOb(xT, yT)));
  cRotate(IREnE.xythetaTOc(xT, yT, thetaT));

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
    Serial.println(F("  COMPUTED"));
    Serial.println();
  #endif
}

void moveToObjThetaDist(){
  xT = IREnE.objThetaDistTOx(thetaT, distT);
  yT = IREnE.objThetaDistTOy(thetaT, distT);
  
  #ifdef DEBUG
    Serial.println(F("   moveToObjThetaDist"));
    Serial.print(F("IREnE.objectX():"));
    Serial.print(IREnE.objectX());
    Serial.print(F("IREnE.objectY():"));
    Serial.print(IREnE.objectY());
    Serial.print(F("distT:"));
    Serial.print(distT);
    Serial.print(F("thetaT:"));
    Serial.println(thetaT);
    Serial.print(F("xT: "));
    Serial.print(xT);
    Serial.print(F(" yT: "));
    Serial.println(yT);
    Serial.println(F("   COMPUTED"));
    Serial.println();
  #endif
  
  moveToxytheta();
}
