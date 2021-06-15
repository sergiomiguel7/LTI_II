import { HttpClient } from '@angular/common/http';
import { Component, OnInit, ViewChild } from '@angular/core';
import { FormControl } from '@angular/forms';
import { MatSort } from '@angular/material/sort';
import { MatTableDataSource } from '@angular/material/table';
import { Chart } from 'chart.js';
import * as moment from 'moment';
import { AdminService } from 'src/app/services/admin.service';
import { ValuesService } from 'src/app/services/values.service';


@Component({
  selector: 'app-admin-page',
  templateUrl: './admin-page.component.html',
  styleUrls: ['./admin-page.component.scss']
})
export class AdminPageComponent implements OnInit {

  @ViewChild(MatSort, { static: false }) sort: MatSort | undefined;

  users: any = [];
  displayedColumns: string[] = ['id', 'username', 'role'];
  displayedColumns2: string[] = ['id', 'owner', 'area'];

  usersTable: any = [];
  concentradoresTable: any = [];

  constructor(private admin: AdminService) { }

  ngOnInit(): void {
    this.getData();
  }

  getData() {
    this.admin.getUsers().subscribe((data: any) => {
      let tableData = [] as any;
      this.users = data.data;
      data.data.forEach((value: any) => {
        tableData.push({
          id: value.id,
          username: value.username,
          role: value.role,
        });
      });
      this.usersTable = new MatTableDataSource<any>(tableData);
      this.usersTable.sort = this.sort;

      this.getConcentradores();
    })

  }

  getConcentradores(){
    this.admin.getConcentradores().subscribe((data: any) => {
      let tableData = [] as any;

      data.data.forEach((value: any) => {
        
        tableData.push({
          id: value.id,
          owner: this.users.find((el:any) => el.id == value.id_user).username,
          area: value.area,
        });
      });

      this.concentradoresTable = new MatTableDataSource<any>(tableData);
      this.concentradoresTable.sort = this.sort;
    });
  }

}
