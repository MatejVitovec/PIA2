S = [0,0];
Sv = [0,0];
Sm = 27068700;

Z = [0,100];
Zv = [50,0];
Zm = 81.30;

M = [0, 100.1];
Mv = [50,0];
Mm = 1;

t = 0;
dt = 0.001;

Zx = zeros(1, 10000);
Zy = zeros(1, 10000);
Mx = zeros(1, 10000);
My = zeros(1, 10000);
k = 0;


while t < 10
    t = t + dt;
    k = k+1;
%     Za = (-pohybovaRovnice(S, Z, Sm, Zm)./Zm);
    Za = (-pohybovaRovnice(S, Z, Sm, Zm)./Zm) + (-pohybovaRovnice(M, Z, Mm, Zm)./Zm);
    Ma = (-pohybovaRovnice(S, M, Sm, Mm)./Mm) + (-pohybovaRovnice(Z, M, Zm, Mm)./Mm);
    
    Zv = Zv + dt.*Za;
    Mv = Mv + dt.*Ma;
    
    Z = Z + dt.*Zv;
    M = M + dt.*Mv;
    
    Zx(k) = Z(1);
    Zy(k) = Z(2);
    Mx(k) = M(1);
    My(k) = M(2);
end

figure
hold on
plot(Zx, Zy);
plot(Mx, My);

hold off

function [F] = pohybovaRovnice(r1, r2, m1, m2) %F vztahuje smer na 2 
    G = 6.674*10^(-11);
    r = sqrt((r2(1)-r1(1))^2+(r2(2)-r1(2))^2);
    r0 = [(r2(1)-r1(1))/r, (r2(2)-r1(2))/r];    
    
    F = G*((m1*m2)/(r^2));
    F = F.*r0;
end
