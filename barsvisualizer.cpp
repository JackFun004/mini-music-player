#include "barsvisualizer.h"
#include "fftw-3.3.5-dll64/fftw3.h"
#include <QPainter>

BarsVisualizer::BarsVisualizer()
    :buffer()
{
    timer = new QTimer(this);
    QObject::connect(timer,&QTimer::timeout,this,[=](){
        this->update();
    });
    for(int i =0;i<BarsNumber;i++){
        h_values[i]=0;
        h_drop[i]=0;
    }
}

void BarsVisualizer::pause()
{
    timer->start(30);
}

void BarsVisualizer::paint(QPainter *painter)
{

    painter->setBrush(QBrush(mainColor()));
    painter->setPen(mainColor());
    qreal rect_w = width()/BarsNumber;

    if(timer->isActive()){
        int cnt = 0;
        int i;
        for(i=0;i<BarsNumber/2;i++){
            if(h_values[i]==0){
                cnt++;
            }else{
                h_drop[i] += 1;
                h_values[i] -= h_drop[i];
                if(h_values[i]<0){
                    h_values[i]=0;
                    h_drop[i] = 0;
                    cnt++;
                }
            }
            painter->drawRect((i+(BarsNumber>>1))*rect_w,height()-h_values[i],rect_w,h_values[i]);
        }
        for(;i<BarsNumber;i++){
            if(h_values[i]==0){
                cnt++;
            }else{
                h_drop[i] += 1;
                h_values[i] -= h_drop[i];
                if(h_values[i]<0){
                    h_values[i]=0;
                    h_drop[i] = 0;
                    cnt++;
                }
            }
            painter->drawRect((i-(BarsNumber>>1))*rect_w,height()-h_values[i],rect_w,h_values[i]);
        }
        if(cnt==BarsNumber)
            timer->stop();

        return;
    }

    QAudioBuffer buf_temp = std::move(buffer);
    if(!buf_temp.isValid())
        return;

    float *in = buf_temp.data<float>();
    fftwf_complex *out;
    fftwf_plan p;
    int num = buf_temp.sampleCount();

    out = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex)* (num/2+1));

    p = fftwf_plan_dft_r2c_1d(num,in,out,FFTW_ESTIMATE);

    fftwf_execute(p);

    int points_per_bar = (num/2+1)/BarsNumber;
    int i;
    for(i=0;i<BarsNumber/2;i++){
        int center_i = i*points_per_bar+(points_per_bar>>1);
        int h_value = out[center_i][0]*0.5
                     +(out[center_i+1][0]+out[center_i-1][0])*0.15
                     +(out[center_i+2][0]+out[center_i-2][0])*0.1;
        h_value*=3;
        if(h_value>=h_values[i]){
            h_values[i] = h_value;
            h_drop[i] = 0;
        }else{
            h_drop[i] += 1;
            h_values[i] -= h_drop[i];
            if(h_values[i]<0){
                h_values[i]=0;
            }
        }
        painter->drawRect((i+(BarsNumber>>1))*rect_w,height()-h_values[i],rect_w,h_values[i]);
    }
    for(;i<BarsNumber;i++){
        int center_i = i*points_per_bar+(points_per_bar>>1);
        int h_value = out[center_i][0]*0.5
                      +(out[center_i+1][0]+out[center_i-1][0])*0.15
                      +(out[center_i+2][0]+out[center_i-2][0])*0.1;
        h_value*=3;
        if(h_value>=h_values[i]){
            h_values[i] = h_value;
            h_drop[i] = 0;
        }else{
            h_drop[i] += 1;
            h_values[i] -= h_drop[i];
            if(h_values[i]<0){
                h_values[i]=0;
            }
        }
        painter->drawRect((i-(BarsNumber>>1))*rect_w,height()-h_values[i],rect_w,h_values[i]);
    }
    //clear
    fftwf_destroy_plan(p);
    fftwf_free(out);
}

void BarsVisualizer::visualize(const QAudioBuffer &buf)
{
    timer->stop();
    this->buffer = buf;
    update();
}
