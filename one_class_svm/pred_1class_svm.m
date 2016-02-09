function pred_labels=pred_1class_svm(user1,input_control,fs, ...
                                     test_app_no,threshold,n_classes)

  apps=[1;2;3;4;5];
  users=(1:n_classes)';
  pred_labels=[];
  cr_val_no=4;
  
  [a,b,train_data]=loadSVMDataFromFiles([user1],apps(apps~=test_app_no), ...
                                  input_control,fs);
  [a,b,test_orig]=loadSVMDataFromFiles([user1],test_app_no, ...
                                input_control,fs);
  
  train_label=zeros(size(train_data,1),1)+1;
  train_data_size=size(train_data,1);
  test_orig_label=zeros(size(test_orig,1),1)+1;
  
  test_data=[];
  test_label=[];
  test_sizes=[];
  
  %Start tweaks
  [a,test_imposter_sizes,test_imposter]=loadSVMDataFromFiles(users(users~=user1),test_app_no, ...
                                     input_control,fs);
  %End tweaks
  
  test_imposter_label=zeros(size(test_imposter,1),1)-1;
  test_data=[test_orig;test_imposter];
  test_label=[test_orig_label;test_imposter_label];
  test_sizes=[size(test_orig,1);test_imposter_sizes];
  
  data=[train_data;test_data];
      
  data=(data - repmat(min(data,[],1),size(data,1),1))* ...
    spdiags(1./(max(data,[],1)-min(data,[],1))', ...
      0,size(data,2),size(data,2));
  
  train_data=data(1:train_data_size,:);
  test_data=data(train_data_size+1:end,:);
   
  bestcv = 0;
  bestnu=-5;
  bestg=-5;
  for nu = 0.01:0.005:0.4,
    for log2g = -9:4,
      cmd = ['-q -s 2 -v ',num2str(cr_val_no),' -n ', num2str(nu), ' -g ', num2str(2^log2g)];
      cv = svmtrain(train_label, train_data, cmd);
      if (cv >= bestcv),
        bestcv = cv; bestnu = nu; bestg = 2^log2g;
      end
      %fprintf('%g %g %g (best nu=%g, g=%g, rate=%g)\n', nu, log2g, cv, bestnu, bestg, bestcv);
    end
  end
  %fprintf('**(best nu=%g, g=%g, rate=%g)\n', bestnu, bestg, bestcv);
  
  cmd = ['-q -s 2 -n ', num2str(bestnu), ' -g ', num2str(bestg)];
  model = svmtrain(train_label,train_data,cmd);
  [predict_label,accuracy,dec_values] = svmpredict(test_label, test_data, model);
  %predict_label
  %sum(predict_label(1:11,:)==1)
  %sum(predict_label(12:22,:)==-1)
  if threshold>size(test_orig,1)-1
    threshold=size(test_orig,1)-1;
  end
  K=size(test_orig,1)-threshold;
  for class=1:n_classes
    [start_ind,end_ind]=getSessionForClass(test_sizes,class);
    class_predict_label=predict_label(start_ind:end_ind,1);
    accept=0;
    %if sum(class_true_label==1)==size(class_true_label,1)
    %  %this session usage belongs to true owner
    %  score=sum(class_predict_label==1);
    %  if score < K
    %    %true owner will be rejected
    %    false_rej=false_rej+1;
    %  end
    %else
    %  %this session usage belongs to imposter
    %  score=sum(class_predict_label==-1);
    %  if score >=K
    %    %an imposter will be accepted
    %    false_acc=false_acc+1;
    %  end
    %end 
    score=sum(class_predict_label==1);
    if score < K
      predicted_class=-1;
    else
      predicted_class=1;
    end
    pred_labels=[pred_labels;predicted_class];
  end %end for

end
