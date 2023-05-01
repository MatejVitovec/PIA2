
S = [0,0];
Sv = [0,0];
Sm = 1;

Z = [-0.1667743823220, 0.9690675883429];
Zv = [-0.0172346557280, -0.0029762680930];
Zm = 3.00348959632*10^(-6);

M = [-0.1694619061456, 0.9692330175719];
Mv = [-0.0172817331582, -0.0035325102831];
Mm = (1.23000383*10^(-2))*Zm;

t = 0;
dt = 0.001;

Zx = zeros(1, 100); %pro vykresleni
Zy = zeros(1, 100); %pro vykresleni
Mx = zeros(1, 100); %pro vykresleni
My = zeros(1, 100); %pro vykresleni
k = 0; %pro vykresleni


while t < 365
    t = t + dt;
    k = k + 1; %pro vykresleni

    Za = (-pohybovaRovnice(S, Z, Sm, Zm)./Zm) + (-pohybovaRovnice(M, Z, Mm, Zm)./Zm);
    Ma = (-pohybovaRovnice(S, M, Sm, Mm)./Mm) + (-pohybovaRovnice(Z, M, Zm, Mm)./Mm);
    
    Zv = Zv + dt.*Za;
    Mv = Mv + dt.*Ma;
    
    Z = Z + dt.*Zv;
    M = M + dt.*Mv;
    
    Zx(k) = Z(1); %pro vykresleni
    Zy(k) = Z(2); %pro vykresleni
    Mx(k) = M(1); %pro vykresleni
    My(k) = M(2); %pro vykresleni
end

%pro vykresleni
figure
hold on
plot(Zx, Zy);
plot(Mx, My);
plot(0,0,'r*');
hold off

function [F] = pohybovaRovnice(r1, r2, m1, m2) %F vztahuje smer na 2 
    G =  2.95912208286*10^(-4);
    r = sqrt((r2(1)-r1(1))^2+(r2(2)-r1(2))^2);
    r0 = [(r2(1)-r1(1))/r, (r2(2)-r1(2))/r];    
    
    F = G*((m1*m2)/(r^2));
    F = F.*r0;
end



%TODO
function [f] = func()
    



end


function [u] = explicitEuler(maxIter, dt, u0, f)
    u = u0;
    un = u0;
    iter = 0;

    while iter <= maxIter
        un = u + dt.*f;
        u = un;
        iter = iter + 1;
    end

end
