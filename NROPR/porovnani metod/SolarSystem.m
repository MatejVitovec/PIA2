clear all

global masses
global G
global numRows
global numCols

data = fileread("data.txt");
data = split(splitlines(data));
[numRows,numCols] = size(data);

names = string(data(2:numRows, 2));
initialCondition = str2double(data(2:numRows, 3:8));
masses = str2double(data(2:numRows, 9));
planetNum = numRows - 1;

G = 6.67e-11;

% 273 yars
% tspan  = [0, 5206800000];
% [t, y] = ode45(@F, tspan, reshape(initialCondition',[],1));
% [t, y] = ode113(@F, tspan, reshape(initialCondition',[],1));
% [t, y] = ode78(@F, tspan, reshape(initialCondition',[],1));
[t, y] = ode4(@F, 0:60000:5199600000, reshape(initialCondition',[],1));




[yRows,yCols] = size(y);

out = reshape(y, yRows, 6, planetNum);

A(:,:) = out(yRows, :, :);

mars(:,:) = out(:, :, 2);
sun(:,:) = out(:, :, 1);

merkurDist = zeros(length(t), 1);
% mars vzdalenost
i = 1;
while i <= length(merkurDist)
    merkurDist(i) = distance(mars(i,:), sun(i, :));
    i = i + 1;
end

figure
hold on
plot(t, merkurDist)
yl = ylim; % Get current limits.
ylim([0, yl(2)]);
hold off

figure
hold on
axis equal

plot3(out(:, 1, 1), out(:, 2, 1),  out(:, 3, 1));
plot3(out(:, 1, 2), out(:, 2, 2),  out(:, 3, 2));
plot3(out(:, 1, 3), out(:, 2, 3),  out(:, 3, 3));
plot3(out(:, 1, 4), out(:, 2, 4),  out(:, 3, 4));
plot3(out(:, 1, 5), out(:, 2, 5),  out(:, 3, 5));
plot3(out(:, 1, 6), out(:, 2, 6),  out(:, 3, 6));
plot3(out(:, 1, 7), out(:, 2, 7),  out(:, 3, 7));
plot3(out(:, 1, 8), out(:, 2, 8),  out(:, 3, 8));
plot3(out(:, 1, 9), out(:, 2, 9),  out(:, 3, 9));
plot3(out(:, 1, 10), out(:, 2, 10),  out(:, 3, 10));

legend(names);


function [f] = F(~, u)
    global masses
    global G
    global numRows
    global numCols

    planetNum = numRows - 1;

    f = zeros(planetNum * 6, 1);

    index = 1;

    while index <= planetNum
    
        f(1+(index-1)*6) = u(4+(index-1)*6);
        f(2+(index-1)*6) = u(5+(index-1)*6);
        f(3+(index-1)*6) = u(6+(index-1)*6);
        f(4+(index-1)*6) = 0;
        f(5+(index-1)*6) = 0;
        f(6+(index-1)*6) = 0;
    
        i = 1;
        while i <= planetNum
            if i == index
                i = i + 1;
                continue;
            end
            dist = distance(u((1+(i-1)*6):(3+(i-1)*6)), u((1+(index-1)*6):(3+(index-1)*6)));
            distance3 = dist^3;
            f(4+(index-1)*6) = f(4+(index-1)*6) + G*masses(i)*((u(1+(i-1)*6) - u(1+(index-1)*6))/distance3);
            f(5+(index-1)*6) = f(5+(index-1)*6) + G*masses(i)*((u(2+(i-1)*6) - u(2+(index-1)*6))/distance3);
            f(6+(index-1)*6) = f(6+(index-1)*6) + G*masses(i)*((u(3+(i-1)*6) - u(3+(index-1)*6))/distance3);
            i = i + 1;
        end
        index = index + 1;
    end
end

function [d] = distance(u1, u2)
    d = sqrt((u1(1) - u2(1))^2 + (u1(2) - u2(2))^2 + (u1(3) - u2(3))^2);
end


function [t, y] = ode4(odefun, tspan, y0)
    t = tspan;
    h = tspan(2) - tspan(1);
    
    y = zeros(length(y0), length(t));
    y(:,1) = y0;
    
    for i = 1:(length(t)-1)
        k1 = odefun(t(i), y(:,i));
        k2 = odefun(t(i) + 0.5*h, y(:,i) + 0.5*h*k1);
        k3 = odefun((t(i) + 0.5*h), (y(:,i) + 0.5*h*k2));
        k4 = odefun((t(i) + h), (y(:,i) + k3*h));
        y(:,i+1) = y(:,i) + (1/6)*(k1 + 2*k2 + 2*k3 + k4)*h;
    end

    y = y.';
end






