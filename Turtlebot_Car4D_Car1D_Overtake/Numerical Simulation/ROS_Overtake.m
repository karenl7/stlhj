clear all
global deriv deriv_R deriv_L gmatOut always_alpha
load deriv.mat 
load derivLR.mat 
load always_alpha.mat
% %rosshutdown
rosinit('http://joe.local:11311');
%rosinit
global ctrlpub msg

ctrlpub = rospublisher('/ctrl_MATLAB','std_msgs/Float64MultiArray');
msg = rosmessage(ctrlpub);

rossubscriber('/StateSpace',@ctrl_for_joe);

function ctrl_for_joe(~,message)
global deriv deriv_R deriv_L gmatOut
global ctrlpub msg
x = message.Data(1);%message.x;
y = message.Data(2);%message.y;
%th = wrapTo2Pi(message.Data(3));%message.theta;
th = message.Data(3);
V = message.Data(4);%message.V;
y2 = message.Data(5);%message.y2;
t = message.Data(6);

if t>25
    t = 25;
end
tstep_sim = 0.25;
i = floor(t/tstep_sim); %which alpha_U_beta to evaluate

traj = [x,y,th,V,y2];

[~,value_always] = eval_u(traj,gmatOut,always_alpha{end+2-i}(:,:,:,:,:),...
    always_alpha{end+2-i}(:,:,:,:,:),tstep_sim,0);

if value_always >= 0 
    %compute gradient of value function and control action
    [u,~,~] = eval_u_deriv(traj,gmatOut,deriv,deriv_R,deriv_L,0,NaN,i+2);
else
    %LFRCS 
    [u,~,~] = eval_u_deriv_halfplane(traj,gmatOut,deriv,deriv_R,deriv_L,tstep_sim,0,i+2,intercept_Coefficient,slope_Coefficient);                
end

u = [u(2),u(1)]';

if isempty(find(isnan(u),1))==0
    fprintf("error!")
end
u(isnan(u)) = 0;
msg.Data = double(u);
send(ctrlpub,msg);

end

%rosshutdown

