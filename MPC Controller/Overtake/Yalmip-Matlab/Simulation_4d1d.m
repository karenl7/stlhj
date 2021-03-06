clear all
%load alpha_U_beta.mat
load overtake_output.mat
[gmat,datamat] = cpp2matG(g,alpha_U_beta);
gmatOut = processGrid(gmat,datamat(:,:,:,:,:,1));

deriv_3 = computeGradients_C(gmatOut,single(datamat),[0,0,1,0,0]);
deriv_4 = computeGradients_C(gmatOut,single(datamat),[0,0,0,1,0]);
deriv_3{4} = deriv_4{4};
deriv = deriv_3;


save('deriv.mat','deriv','gmatOut')


clear all
%load alpha_U_beta.mat
load overtake_output.mat
[gmat,datamat] = cpp2matG(g,alpha_U_beta);
gmatOut = processGrid(gmat,datamat(:,:,:,:,:,1));

%[~,deriv_3_L,~] = computeGradients_L(gmatOut,single(datamat),[0,0,1,0,0]);
deriv_3_L = computeGradients_L(gmatOut,single(datamat),[0,0,1,0,0]);
deriv_4_L = computeGradients_L(gmatOut,single(datamat),[0,0,0,1,0]);
deriv_3_L{4} = deriv_4_L{4};
deriv_L = deriv_3_L;

%deriv_3_R = computeGradients_R(gmatOut,single(datamat),[0,0,1,0,0]);
deriv_3_R = computeGradients_R(gmatOut,single(datamat),[0,0,1,0,0]);
deriv_4_R = computeGradients_R(gmatOut,single(datamat),[0,0,0,1,0]);
deriv_3_R{4} = deriv_4_R{4};
deriv_R = deriv_3_R;



save('derivLR.mat','deriv_L','deriv_R','gmatOut')
%%
clear all
% close all
%load alpha_U_beta.mat
load overtake_output.mat
load deriv.mat
load derivLR.mat
%T = 25; %total time
T=25;
tstep = T/(length(alpha_U_beta)-1); %timestep between aUb frames
tstep_sim = 0.125/2; %timestep between control evaluations
t_real = 0:tstep:T; 
u = zeros(tstep/tstep_sim,3);

traj = cell(numel(t_real),1);
%traj_temp = [-0.1,0.,pi/2,0,0.8];
traj_temp = [.2,-0.55,90*pi/180,0.0,-0.55];
t = cell(numel(t_real),1);
t_temp = 0;
t{1} = t_temp;
value_function = cell(numel(t_real),1); %store values
value_function{1} = [];
Vq_V = cell(numel(t_real),1); %store values
Vq_V{1} = [];
Vq_th = cell(numel(t_real),1); %store values
Vq_th{1} = [];
t_vf = cell(numel(t_real),1); %store values
t_vf{1} = [];
u_mat = cell(numel(t_real),1);

u(:,3) = 0.15; %second car's velocity
value = 0.04;

for i = 1:numel(t_real)
    if i == 1
        [~,value] = eval_u(traj_temp(end,:),gmatOut,alpha_U_beta{end+1-i},alpha_U_beta{end+1-i},tstep_sim,value);
       
        [~,dVq_V,dVq_th] = eval_u(traj_temp(end,:),gmatOut,deriv{3}(:,:,:,:,:,i+1),...
            deriv{4}(:,:,:,:,:,end+1-i),tstep_sim,value);
        
        t{i} = t_real(i);
        Vq_V{i} = dVq_V;
        Vq_th{i} = dVq_th;
        t_vf{i} = t_real(i);
        value_function{i} = value;
        traj{i} = traj_temp; %initial state
    else
        for j = 1:tstep/tstep_sim
            %compute value function value at current position
            [~,value] = eval_u(traj_temp(end,:),gmatOut,alpha_U_beta{end+2-i}(:,:,:,:,:),alpha_U_beta{end+2-i}(:,:,:,:,:),tstep_sim,value);
            
            %compute gradient of value function and control action
            [u(j,1:2),dVq_V,dVq_th] = eval_u_deriv(traj_temp(end,:),gmatOut,deriv,deriv_R,deriv_L,tstep_sim,value,i+1);
            
            %compute trajectory
            [t_temp,traj_temp] = ode45(@(t_temp,traj_temp) odefun_dubinsCar(t_temp,traj_temp,u(j,:)),...
                [t_temp(end) t_temp(end)+tstep_sim],traj_temp(end,:));
            t{i} = [t{i};t_temp(2:end)];
            
            Car2_bounds = find(traj_temp(:,5) >= 0.7);
            traj_temp(Car2_bounds,5) = 0.7;
            
            %store value function gradient
            Vq_V{i} = [Vq_V{i} dVq_V];
            Vq_th{i} = [Vq_th{i} dVq_th];
            t_vf{i} = [t_vf{i} t_temp(1)];
            
            %store value function values
            value_function{i} = [value_function{i} value];
            
            %store trajectory
            traj{i} = [traj{i};traj_temp(2:end,:)];
%             if traj_temp(end,1)>=12 || traj_temp(end,1)<=-12 || traj_temp(end,2)>=12 || traj_temp(end,2)<=-12
%                 break
%             end
        end
    end
    
    u_mat{i} = u;
end

u_mat{1} = u_mat{1}(1,:);


save("traj.mat","traj","value_function","Vq_V","Vq_th","t_vf","t","u_mat")

%load overtake_output.mat
% %load alpha_U_beta.mat
% %load traj_pass3.mat
% load traj.mat
T = 25;
%T= 25;%t_mat = linspace(1,81,9);
%t_mat = [1 14 26 39 51 64 76 89 101];
t_mat = ceil(linspace(1,101,4));
%t_mat = [1,17,33,49]
%t_mat = [1,3,5,7,9,11,13];
y2 = zeros(numel(t_mat),1);
for i = 1:numel(t_mat)
    y2(i) = traj{t_mat(i)}(end,5);
end

V2_I = round(interpn(linspace(g.min(5),g.max(5),g.N(5)),1:g.N(5),y2));
V2_I(isnan(V2_I))=min(V2_I);
caxismin = -3;
caxismax = 3.;


close all; figure;
set(gcf, 'Position', [100    44   936   790])
for k = 1:numel(t_mat)
    
    %[gmat,datamat] = cpp2matG(g,alpha_U_beta_d4(k));
    %g[gmat2] = processGrid(gmat);
    %gmat2.dim = 4;
    %[g2d,data2d] = proj(gmat2,datamat,[0,0,1,1],'max');
    if k == 1
        [gmat,datamat] = cpp2matG(g,alpha_U_beta(end+1-t_mat(k)));
        [gmat2] = processGrid(gmat);
        [g2d,data2d] = proj(gmat2,datamat,[0,0,1,1,1],[traj{t_mat(k)}(1,3),traj{t_mat(k)}(end,4),traj{t_mat(k)}(end,5)]);
        %[g2d,data2d] = proj(gmat2,datamat,[0,0,1,1,1],'max');
    else
        [gmat,datamat] = cpp2matG(g,alpha_U_beta(end+2-t_mat(k)));
        [gmat2] = processGrid(gmat);
        [g2d,data2d] = proj(gmat2,datamat,[0,0,1,1,1],[traj{t_mat(k)}(1,3),traj{t_mat(k)}(end,4),traj{t_mat(k)}(end,5)]);
        %[g2d,data2d] = proj(gmat2,datamat,[0,0,1,1,1],'max');

    end
    subplot(2,2,k)
    X = linspace(g2d.min(1),g2d.max(1),g2d.N(1));
    Y = linspace(g2d.min(2),g2d.max(2),g2d.N(2));
    [~,h]=contourf(X,Y,data2d',[-3:0.1:3]);
    colorbar;
    caxis([caxismin,caxismax])
    hold on;
    [~,h2] = contour(X,Y,data2d',[0 0],'ShowText','on');
    set(h2,'LineColor','k');
    xlabel('x (m)')
    ylabel('y (m)')
    title(['\theta=' num2str(traj{t_mat(k)}(end,3)*180/pi,'%.0f') '^o, t=' num2str(((t_mat(k)-1)*T)/(length(alpha_U_beta)-1),'%.0f') 's'])
    set(h,'LineColor','none');
    axis equal
    axis([-0.4 0.4 -0.6 0.6])
    %plot([0.2,-0.4],[-0.4,0.2],'xk','Markersize',15,'LineWidth',6);
    



    %subplot(2,2,k); hold on;
    
    max_j = t_mat(k);
    if y2(k) < 0.9
    plot(-0.2,y2(k),'ok','MarkerSize',15,'MarkerFaceColor','red');
    end

    for j = 1:max_j
        plot(traj{j}(:,1),traj{j}(:,2),'LineWidth',4)
    end
    plot(traj{max_j}(end,1),traj{max_j}(end,2),'ok','MarkerSize',15,'MarkerFaceColor','white')
    [x_arrow,y_arrow] = pol2cart(traj{max_j}(end,3),0.1);
    base_x = traj{max_j}(end,1);
    base_y = traj{max_j}(end,2);
    
    rho = 0.2;
    %rho_scale = 0.2/0.15;
    %rho = rho_scale*traj{max_j}(end,4);
    q = quiver(base_x,base_y,rho*cos(traj{max_j}(end,3)),rho*sin(traj{max_j}(end,3)));
    q.Color = 'black';
    q.LineWidth = 2;
    q.MaxHeadSize = 1;
    q.AutoScale = 'off';
    
    rho2 = rho;
    %rho2 = rho_scale*u(k,3);
    base_y2 = traj{max_j}(end,5);
    q2 = quiver(-0.2,base_y2,0,rho2);
    q2.Color = 'black';
    q2.LineWidth = 2;
    q2.MaxHeadSize = 1;
    q2.AutoScale = 'off';
    
    ax = gca;
    ax.FontSize = 15;
    
end

%%
figure; hold on
for i=1:length(alpha_U_beta)
    plot(t_vf{i},value_function{i},'-o')
end
title('Value function')

figure; hold on
for i = 1:length(alpha_U_beta)
    plot(t_vf{i},Vq_V{i},'-o')
    %legend_cell{i}=['t = ' num2str(t_vf{i}(1)) 's'];
end
title('dV(x dot)/dt(t,x(t)) vs t')
xlabel('t')
ylabel('dV(x dot)/dt(t,x(t))')

figure; hold on
for i = 1:length(alpha_U_beta)
    plot(t_vf{i},Vq_th{i},'-o')
    %legend_cell{i}=['t = ' num2str(t_vf{i}(1)) 's'];
end
title('dV(\theta)/dt(t,x(t)) vs t')
xlabel('t')
ylabel('dV(\theta)/dt(t,x(t))')

%%
figure; subplot(6,1,1); hold on
for i = 1:length(alpha_U_beta)
    plot(t_vf{i},u_mat{i}(:,2),'-o')
    %legend_cell{i}=['t = ' num2str(t_vf{i}(1)) 's'];
end
title('acceleration(t) vs t')
xlabel('t')
ylabel('a(t)')

subplot(6,1,2); hold on
for i = 1:length(alpha_U_beta)
    plot(t{i},traj{i}(:,4))
end
title('velocity(t) vs t')
xlabel('t')
ylabel('v(t)')

subplot(6,1,3); hold on
for i = 1:length(alpha_U_beta)
    plot(t{i},traj{i}(:,3))
end
title('\theta(t) vs t')
xlabel('t')
ylabel('\theta(t)')

subplot(6,1,4); hold on
for i = 1:length(alpha_U_beta)
    plot(t_vf{i},Vq_V{i},'-o')
    %legend_cell{i}=['t = ' num2str(t_vf{i}(1)) 's'];
end
title('dV(x dot)/dt(t,x(t)) vs t')
xlabel('t')
ylabel('dV(x dot)/dt(t,x(t))')

subplot(6,1,5); hold on
for i = 1:length(alpha_U_beta)
    plot(t_vf{i},Vq_th{i},'-o')
    %legend_cell{i}=['t = ' num2str(t_vf{i}(1)) 's'];
end
title('dV(\theta)/dt(t,x(t)) vs t')
xlabel('t')
ylabel('dV(\theta)/dt(t,x(t))')

subplot(6,1,6); hold on
for i=1:length(alpha_U_beta)
    plot(t_vf{i},value_function{i},'-o')
end
title('Value function')


%%
figure; subplot(2,1,1); hold on
for i = 1:length(alpha_U_beta)
    plot(t_vf{i},u_mat{i}(:,1),'-o')
    %legend_cell{i}=['t = ' num2str(t_vf{i}(1)) 's'];
end
title('\omega(t) vs t')
xlabel('t')
ylabel('\omega(t)')

subplot(2,1,2); hold on
for i = 1:length(alpha_U_beta)
    plot(t{i},traj{i}(:,3))
    %legend_cell{i}=['t = ' num2str(t_vf{i}(1)) 's'];
end
title('\theta(t) vs t')
xlabel('t')
ylabel('\theta(t)')

% figure; hold on
% for i = 1:12
%     plot(t_vf{i}-3,value_function{i},'LineWidth',2)
%     %legend_cell{i}=['t = ' num2str(t_vf{i}(1)) 's'];
% end
% title('V(t,x(t)) vs t')
% xlabel('t')
% ylabel('V(t,x(t))')
% %legend(legend_cell)
% run plotfixer.m

%%
% subplot(2,3,1); hold on;
%     for j = 1:max_j-1
%         plot(traj{j}(:,1),traj{j}(:,2),'k','LineWidth',2)
%     end

%%
 figure; hold on;
i = length(alpha_U_beta)-30;
traj_check = traj{i}(5,:);
th_mat = linspace(g.min(3),g.max(3),g.N(3));
value_check = zeros(numel(th_mat),1);
for j = 1:numel(th_mat)
    traj_check(3) = th_mat(j);
    [~,value_check(j)] = eval_u(traj_check,gmatOut,alpha_U_beta{end+2-i}(:,:,:,:,:),alpha_U_beta{end+2-i}(:,:,:,:,:),tstep_sim,value);
end
plot(th_mat*180/pi,value_check,'o')

%%
 figure; hold on;
i = length(alpha_U_beta)-30;
traj_check = traj{i}(5,:);
v_mat = linspace(g.min(4),g.max(4),g.N(4));
value_check = zeros(numel(v_mat),1);
for j = 1:numel(v_mat)
    traj_check(4) = v_mat(j);
    [~,value_check(j)] = eval_u(traj_check,gmatOut,alpha_U_beta{end+2-i}(:,:,:,:,:),alpha_U_beta{end+2-i}(:,:,:,:,:),tstep_sim,value);
end
plot(v_mat,value_check,'o')

%%
figure; hold on;
i = length(alpha_U_beta);
traj_check = traj{i}(5,:);
th_mat = linspace(g.min(3),g.max(3),g.N(3));
dVq_th_check = zeros(numel(th_mat),1);
for j = 1:numel(th_mat)
    traj_check(3) = th_mat(j);
    [u(j,1:2),dVq_V,dVq_th_check(j)] =eval_u_deriv(traj_temp(end,:),gmatOut,deriv,deriv_R,deriv_L,tstep_sim,value,i+1);
end
plot(th_mat*180/pi,dVq_th_check,'.')
grid on
%ylim([min(dVq_th_check),max(dVq_th_check)])

