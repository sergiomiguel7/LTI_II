import { HttpClient } from '@angular/common/http';
import { Component, OnInit, ViewChild } from '@angular/core';
import { FormControl } from '@angular/forms';
import { MatSort } from '@angular/material/sort';
import { MatTableDataSource } from '@angular/material/table';
import { Chart } from 'chart.js';
import * as moment from 'moment';
import { ValuesService } from 'src/app/services/values.service';

@Component({
  selector: 'app-data-visual',
  templateUrl: './data-visual.component.html',
  styleUrls: ['./data-visual.component.scss']
})
export class DataVisualComponent implements OnInit {

  @ViewChild(MatSort, { static: false }) sort: MatSort | undefined;


  totalValues: number = 0;
  limit: number = 10;
  page: number = 1;
  pageSizeOptions: number[] = [10, 25, 50];
  displayedColumns: string[] = ['date', 'valor'];

  selectedConcentrador = new FormControl();
  selectedSensor = new FormControl();
  selectedState = new FormControl();

  sensores: any;
  idSensor: number | any;

  concentradores: any;
  idConcentrador: number | any;

  states = [
  {id: 0, label: 'Desligado'},
  {id: 1, label: 'Ligado'},
  {id: 2, label: 'Deteção de movimento'}];
  idState: number | any;

  chartData: any;
  tableData: any = [];
  chart: any;

  showChart: boolean = true;
  showTable: boolean = true;

  constructor(private http: HttpClient,
    private valuesService: ValuesService) {
  }

  ngOnInit(): void {
    this.getConcentradores();
  }

  getConcentradores() {
    this.valuesService.getConcentradores().subscribe((data: any) => {
      this.concentradores = data.data;
    });
  }

  getSensores(){
    this.valuesService.getSensores(this.idConcentrador).subscribe((data: any) => {
      this.sensores = [];      
      this.sensores = data.data;
      this.getValues();
    });
  }

  /**
   * @param event - on select event
   * change the selected concentrador
   */
  changeItem(event: any) {
    this.idConcentrador = event.id;
    this.idSensor = '';
    this.getSensores();
  }


  /**
   * @param event - on select event
   * change the selected sensor
   */
  selectSensor(event: any){
    this.idSensor = event.id;
    this.getValues();
  }


  /**
   * 
   * @param event 
   */
  changeState(event:any){
    console.log("event", event);
    this.idState = event.id;

    let body = {
      iss: this.idSensor,
      state: this.idState
    }

    this.valuesService.changeLight(body).subscribe((data) => {
      if(data)
        console.log("change with sucess");
        
    })
  }

  /**
   * @param event - pagination event
   * get the values from selected filters
   */
  getValues(event?: any) {
    let page = event ? event.pageIndex : this.page;
    let limit = event ? event.pageSize : this.limit;
    let query = this.idSensor ? `?id_sensor=${this.idSensor}` : '';

    this.http.get(`http://localhost:5000/api/values/${this.idConcentrador}${query}`).subscribe((data: any) => {

      let tableData = [] as any;
      this.totalValues = data.storedValues;
      let labels: any[] = [];
      let dataSet: any[] = [];

      //get data from each sensor
      data.data.forEach((el: any) => {

          let date = moment(new Date(el.timestamp * 1000)).format("DD/MM/YYYY HH:mm");
          //chart
          labels.push(date);
          dataSet.push(el.valor);

          //table
          tableData.push({
            date: date,
            valor: el.valor,
          });

      });

      this.chartData = {
        data: dataSet,
        labels: labels
      }

      this.tableData = new MatTableDataSource<any>(tableData);

      this.tableData.sort = this.sort;
      this.buildChart();

    })
  }

  buildChart() {
    var ctx = document.getElementById('myChart') as any;

    if (this.chart !== undefined) {
      this.chart.destroy();
    }

    this.chart = new Chart(ctx, {
      type: 'bar',
      data: {
        labels: this.chartData.labels,
        datasets: [{
          label: 'Valores Lux',
          data: this.chartData.data,
          borderWidth: 1,
          backgroundColor: 'rgba(54, 162, 235, 0.2)',
          borderColor: 'rgb(54, 162, 235)',
        }]
      },
      options: {
        responsive: true,
        maintainAspectRatio: false,
        scales: {
          yAxes: [{
            ticks: {
              beginAtZero: true,
              suggestedMax: 3
            }
          }]
        },
        plugins: {
          datalabels: {
            display: false
          }
        },
      }
    });
  }

  toggle(event: any, type: any) {
    let value = event.checked;
    switch (type) {
      case 'chart': this.showChart = value;
        this.buildChart();
        break;
      case 'table': this.showTable = value;
        break;
    }
  }
}
